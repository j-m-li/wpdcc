/******************************************************************************
 * @file            frag.c
 *****************************************************************************/
#include    "frag.h"
#include    "lib.h"
#include    "section.h"

struct frag zero_address_frag;
struct frag *current_frag = 0;

struct frag *frag_alloc (void) {
	memset(&zero_address_frag,0,sizeof(struct frag));
    return xmalloc (sizeof (struct frag));
}

int frags_is_greater_than_offset (uint64_t offset2, struct frag *frag2, uint64_t offset1, struct frag *frag1, int64_t *offset_p) {

    int64_t difference;
    struct frag *frag;
    
    /* Checks for something that should be impossible. */
    if (frag2 == frag1 || offset1 > frag1->fixed_size) {
        return 0;
    }
    
    difference = offset2 - offset1;
    
    for (frag = frag1;;) {
    
        difference += frag->fixed_size;
        frag = frag->next;
        
        if (frag == frag2) {
        
            if (difference == 0) {
                return 0;
            }
            
            break;
        
        }
        
        if (!frag) {
            return 0;
        }
    
    }
    
    *offset_p = offset2 - offset1 - difference;
    return 1;

}

int frags_offset_is_fixed (struct frag *frag1, struct frag *frag2, int64_t *offset_p) {

    int64_t offset = frag1->address - frag2->address;
    struct frag *frag;
    
    if (frag1 == frag2) {
    
        *offset_p = offset;
        return 1;
    
    }
    
    /* Checks if frag2 is after frag1. */
    frag = frag1;
    
    while (frag->relax_type == RELAX_TYPE_NONE_NEEDED) {
    
        offset += frag->fixed_size;
        
        if (!(frag = frag->next)) {
            break;
        }
        
        if (frag == frag2) {
        
            *offset_p = offset;
            return 1;
        
        }
    
    }
    
    /* Checks if frag1 is after frag2. */
    offset = frag1->address - frag2->address;
    frag = frag2;
    
    while (frag->relax_type == RELAX_TYPE_NONE_NEEDED) {
    
        offset -= frag->fixed_size;
        
        if (!(frag = frag->next)) {
            break;
        }
        
        if (frag == frag1) {
        
            *offset_p = offset;
            return 1;
        
        }
    
    }
    
    return 0;

}

unsigned char *finished_frag_increase_fixed_size_by_frag_offset (struct frag *frag) {

    frag->fixed_size += frag->offset;
    
    if (frag->fixed_size > frag->size) {
    
        frag->buf = xrealloc (frag->buf, frag->fixed_size);
        frag->size = frag->fixed_size;
    
    }
    
    return (frag->buf + frag->fixed_size - frag->offset);

}

unsigned char *frag_alloc_space (uint64_t space) {

    if (current_frag->fixed_size + space >= current_frag->size) {
    
        current_frag->size += ((space > FRAG_BUF_REALLOC_STEP) ? space : FRAG_BUF_REALLOC_STEP);
        current_frag->buf   = xrealloc (current_frag->buf, current_frag->size);
    
    }
    
    return current_frag->buf + current_frag->fixed_size;

}

unsigned char *frag_increase_fixed_size (uint64_t increase) {

    frag_alloc_space (increase);
    
    current_frag->fixed_size += increase;
    return (current_frag->buf + current_frag->fixed_size - increase);

}

void frag_align (int64_t alignment, int fill_char, int64_t max_bytes_to_skip) {

    (frag_alloc_space (1 << alignment))[0] = fill_char;
    frag_set_as_variant (RELAX_TYPE_ALIGN, max_bytes_to_skip, (void*)0, alignment, 0);

}

void frag_align_code (int64_t alignment, int64_t max_bytes_to_skip) {

    (frag_alloc_space (1 << alignment))[0] = 0x90;
    frag_set_as_variant (RELAX_TYPE_ALIGN_CODE, max_bytes_to_skip, (void*)0, alignment, 0);

}

void frag_append_1_char (unsigned char ch) {

    if (current_frag->fixed_size == current_frag->size) {
    
        current_frag->size += FRAG_BUF_REALLOC_STEP;
        current_frag->buf   = xrealloc (current_frag->buf, current_frag->size);
    
    }
    
    current_frag->buf[current_frag->fixed_size++] = ch;

}

void frag_new (void) {

    struct frag *prev_frag = current_frag;
    
    current_frag = frag_alloc ();
    current_frag->relax_type = RELAX_TYPE_NONE_NEEDED;
    
    prev_frag->next = current_frag;
    current_frag_chain->last_frag = current_frag;

}

void frag_set_as_variant (int relax_type, int relax_subtype, struct symbol *symbol, int64_t offset, uint64_t opcode_offset_in_buf) {

    current_frag->relax_type           = relax_type;
    current_frag->relax_subtype        = relax_subtype;
    current_frag->symbol               = symbol;
    current_frag->offset               = offset;
    current_frag->opcode_offset_in_buf = opcode_offset_in_buf;
    
    get_filename_and_line_number (&(current_frag->filename), &(current_frag->line_number));
    frag_new ();

}

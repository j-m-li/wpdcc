/******************************************************************************
 * @file            frag.h
 *****************************************************************************/
#ifndef     _FRAG_H
#define     _FRAG_H

#include    "stdint.h"

#define     RELAX_TYPE_NONE_NEEDED                          0
#define     RELAX_TYPE_ALIGN                                1
#define     RELAX_TYPE_ALIGN_CODE                           2
#define     RELAX_TYPE_ORG                                  3
#define     RELAX_TYPE_SPACE                                4
#define     RELAX_TYPE_MACHINE_DEPENDENT                    5

struct frag {

    uint64_t fixed_size, address, size;
    unsigned int relax_type, relax_subtype;
    
    struct symbol *symbol;
    unsigned char *buf;
    
    int64_t offset;
    uint64_t opcode_offset_in_buf;
    
    const char *filename;
    unsigned long line_number;
    
    int relax_marker;
    struct frag *next;

};

extern struct frag zero_address_frag;
extern struct frag *current_frag;

#define     FRAG_BUF_REALLOC_STEP       16

struct frag *frag_alloc (void);

int frags_is_greater_than_offset (uint64_t offset2, struct frag *frag2, uint64_t offset1, struct frag *frag1, int64_t *offset_p);
int frags_offset_is_fixed (struct frag *frag1, struct frag *frag2, int64_t *offset_p);

unsigned char *finished_frag_increase_fixed_size_by_frag_offset (struct frag *frag);
unsigned char *frag_alloc_space (uint64_t space);
unsigned char *frag_increase_fixed_size (uint64_t increase);

void frag_align (int64_t alignment, int fill_char, int64_t max_bytes_to_skip);
void frag_align_code (int64_t alignment, int64_t max_bytes_to_skip);

void frag_append_1_char (unsigned char ch);
void frag_new (void);
void frag_set_as_variant (int relax_type, int relax_subtype, struct symbol *symbol, int64_t offset, uint64_t opcode_offset_in_buf);

#endif      /* _FRAG_H */

/******************************************************************************
 * @file            section.c
 *****************************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

#include    "as.h"
#include    "frag.h"
#include    "lib.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"

struct section {

    const char *name;
    void *object_format_dependent_data;
    
    struct frag_chain *frag_chain;
    struct symbol *symbol;
    
    unsigned int number;
    unsigned long flags;
    
    struct section *next;

};

static struct section internal_sections[4];
static struct symbol section_symbols[4];

struct section *undefined_section;
struct section *absolute_section;
struct section *expr_section;
struct section *reg_section;

struct section *text_section;
struct section *data_section;
struct section *bss_section;

struct section *current_section;

struct frag_chain *current_frag_chain = 0;
struct section *sections = 0;

unsigned long current_subsection;

static struct section *find_or_make_section_by_name (const char *name) {

    struct section *section, **p_next;
    
    for (p_next = &sections, section = sections; section; p_next = &(section->next), section = *p_next) {
        
        if (strcmp (name, section->name) == 0) {
            break;
        }
    
    }
    
    if (!section) {
    
        section = xmalloc (sizeof (*section));
        section->name = xstrdup (name);
        
        section->symbol = symbol_create (name, section, 0, &zero_address_frag);
        section->symbol->flags |= SYMBOL_FLAG_SECTION_SYMBOL;
        
        symbol_add_to_chain (section->symbol);
        *p_next = section;
    
    }
    
    return section;

}

struct section *section_get_next_section (struct section *section) {
    return section->next;
}

struct section *section_set (struct section *section) {

    struct frag_chain *frag_chain, **p_next;
    unsigned long subsection = 0;
    
    current_section = section;
    
    for (frag_chain = *(p_next = &(current_section->frag_chain)); frag_chain; frag_chain = *(p_next = &(frag_chain->next))) {
    
        if (frag_chain->subsection >= subsection) {
            break;
        }
    
    }
    
    if (!frag_chain || frag_chain->subsection != subsection) {
    
        struct frag_chain *new_frag_chain = xmalloc (sizeof (*new_frag_chain));
        new_frag_chain->subsection = subsection;
        
        new_frag_chain->last_frag  = new_frag_chain->first_frag  = frag_alloc ();
        new_frag_chain->last_fixup = new_frag_chain->first_fixup = 0;
        
        *p_next = new_frag_chain;
        new_frag_chain->next = frag_chain;
        
        frag_chain = new_frag_chain;
    
    }
    
    current_frag_chain = frag_chain;
    current_frag       = current_frag_chain->last_frag;
    
    current_subsection = subsection;
    return section;

}

struct section *section_set_by_name (const char *name) {
    return section_set (find_or_make_section_by_name (name));
}

struct symbol *section_symbol (struct section *section) {
    return section->symbol;
}

const char *section_get_name (struct section *section) {
    return section->name;
}

unsigned int section_get_number (struct section *section) {
    return section->number;
}

unsigned int sections_get_count (void) {

    struct section *section;
    unsigned int count;

    for (section = sections, count = 0; section; section = section->next, count++);
    return count;

}

unsigned long section_get_flags (struct section *section) {
    return section->flags;
}

unsigned long section_write_frag_chain (FILE *fp) {

    struct frag *frag;
    unsigned long bytes = 0;
    
    for (frag = current_frag_chain->first_frag; frag; frag = frag->next) {
    
        if (frag->fixed_size == 0) {
            continue;
        }
        
        if (fwrite (frag->buf, frag->fixed_size, 1, fp) != 1) {
        
            report_at (program_name, 0, REPORT_ERROR, "failed whilst writing data");
            return bytes;
        
        }
        
        bytes += frag->fixed_size;
    
    }
    
    return bytes;

}

void section_set_flags (struct section *section, unsigned long flags) {
    section->flags = flags;
}


#define CREATE_INTERNAL_SECTION(section_var, section_name, section_index) \
    (section_var) = &internal_sections[(section_index)];        \
    (section_var)->name = (section_name);                       \
    (section_var)->symbol = &section_symbols[(section_index)];  \
    (section_var)->symbol->name    = (section_name);            \
    (section_var)->symbol->section = (section_var);             \
    (section_var)->symbol->frag    = &zero_address_frag;        \
    symbol_set_value ((section_var)->symbol, 0);                \
    (section_var)->symbol->flags  |= SYMBOL_FLAG_SECTION_SYMBOL

void sections_init (void) {

    CREATE_INTERNAL_SECTION (undefined_section, "*UND*",    0);
    CREATE_INTERNAL_SECTION (absolute_section,  "*ABS*",    1);
    CREATE_INTERNAL_SECTION (expr_section,      "*EXPR*",   2);
    CREATE_INTERNAL_SECTION (reg_section,       "*REG*",    3);
    
    text_section = section_set_by_name (".text");
    section_set_flags (text_section, SECTION_FLAG_LOAD | SECTION_FLAG_ALLOC | SECTION_FLAG_READONLY | SECTION_FLAG_CODE);
    
    data_section = section_set_by_name (".data");
    section_set_flags (data_section, SECTION_FLAG_LOAD | SECTION_FLAG_ALLOC | SECTION_FLAG_DATA);
    
    bss_section = section_set_by_name (".bss");
    section_set_flags (bss_section, SECTION_FLAG_ALLOC);
    
    /* .text section is the default section. */
    section_set (text_section);

}

#undef CREATE_INTERNAL_SECTION

void section_set_object_format_dependent_data (struct section *section, void *data) {
    section->object_format_dependent_data = data;
}

void sections_number (unsigned int start_at) {

    struct section *section;
    
    for (section = sections; section; section = section->next) {
        section->number = start_at++;
    }

}

void *section_get_object_format_dependent_data (struct section *section) {
    return section->object_format_dependent_data;
}

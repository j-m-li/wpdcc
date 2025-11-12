/******************************************************************************
 * @file            section.h
 *****************************************************************************/
#ifndef     _SECTION_H
#define     _SECTION_H

#include    <stdio.h>

struct frag_chain {

    struct fixup *first_fixup, *last_fixup;
    struct frag *first_frag, *last_frag;
    
    unsigned long subsection;
    struct frag_chain *next;

};

#define     SECTION_FLAG_ALLOC                              (1U << 0)
#define     SECTION_FLAG_LOAD                               (1U << 1)
#define     SECTION_FLAG_READONLY                           (1U << 2)
#define     SECTION_FLAG_CODE                               (1U << 3)
#define     SECTION_FLAG_DATA                               (1U << 4)
#define     SECTION_FLAG_NEVER_LOAD                         (1U << 5)
#define     SECTION_FLAG_DEBUGGING                          (1U << 6)
#define     SECTION_FLAG_EXCLUDE                            (1U << 7)
#define     SECTION_FLAG_NOREAD                             (1U << 8)
#define     SECTION_FLAG_SHARED                             (1U << 9)
#define     SECTION_FLAG_LINK_ONCE                          (1U << 10)
#define     SECTION_FLAG_LINK_DUPLICATES_DISCARD            (1U << 11)
#define     SECTION_FLAG_LINK_DUPLICATES_ONE_ONLY           (1U << 12)
#define     SECTION_FLAG_LINK_DUPLICATES_SAME_SIZE          (1U << 13)
#define     SECTION_FLAG_LINK_DUPLICATES_SAME_CONTENTS      (1U << 14)

#define     SECTION_IS_NORMAL(section)  \
    ((section != undefined_section) && (section != absolute_section) && (section != expr_section) && (section != reg_section))

extern struct section *undefined_section;
extern struct section *absolute_section;
extern struct section *expr_section;
extern struct section *reg_section;

extern struct section *text_section;
extern struct section *data_section;
extern struct section *bss_section;

extern struct section *current_section;

extern struct frag_chain *current_frag_chain;
extern struct section *sections;

struct section *section_get_next_section (struct section *section);
struct section *section_set (struct section *section);
struct section *section_set_by_name (const char *name);

struct symbol *section_symbol (struct section *section);
const char *section_get_name (struct section *section);

unsigned int section_get_number (struct section *section);
unsigned int sections_get_count (void);

unsigned long section_get_flags (struct section *section);
unsigned long section_write_frag_chain (FILE *outfile);

void section_set_object_format_dependent_data (struct section *section, void *data);
void section_set_flags (struct section *section, unsigned long flags);

void sections_number (unsigned int start_at);
void *section_get_object_format_dependent_data (struct section *section);

#endif      /* _SECTION_H */

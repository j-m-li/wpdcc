/******************************************************************************
 * @file            fixup.h
 *****************************************************************************/
#ifndef     _FIXUP_H
#define     _FIXUP_H

#include    "expr.h"

#define     RELOC_TYPE_DEFAULT          0
#define     RELOC_TYPE_FAR_CALL         1
#define     RELOC_TYPE_RVA              2

struct fixup {

    struct frag *frag;
    int done, fixup_signed;
    
    unsigned long where;
    unsigned int size;
    
    struct symbol *add_symbol, *sub_symbol;
    int64_t add_number;
    
    int pcrel, reloc_type;
    struct fixup *next;

};

struct fixup *fixup_new (struct frag *frag, unsigned long where, int size, struct symbol *add_symbol, int64_t add_number, int pcrel, int reloc_type);
struct fixup *fixup_new_expr (struct frag *frag, unsigned long where, int size, struct expr *expr, int pcrel, int reloc_type);

#endif  /* _FIXUP_H */

/******************************************************************************
 * @file            symbol.h
 *****************************************************************************/
#ifndef     _SYMBOL_H
#define     _SYMBOL_H

#include    "expr.h"

#define     SYMBOL_FLAG_EXTERNAL                            0x01
#define     SYMBOL_FLAG_SECTION_SYMBOL                      0x02

struct symbol {

    char *name;
    int scope;
    
    struct section *section;
    struct frag *frag;
    
    struct expr value;
    int resolved, resolving, flags;
    
    unsigned long symbol_table_index;
    int write_name_to_string_table;
    
    struct symbol *next;

};

#define     FAKE_LABEL_NAME             "FAKE_SASM_SYMBOL"
extern struct symbol *symbols;
extern int finalize_symbols;

#define     SYMBOL_SCOPE_LOCAL          0x01
#define     SYMBOL_SCOPE_GLOBAL         0x02
#define     SYMBOL_SCOPE_EXTERN         0x03

struct expr *symbol_get_value_expression (struct symbol *symbol);

char *symbol_get_name (struct symbol *symbol);
int get_symbol_snapshot (struct symbol **symbol_p, uint64_t *value_p, struct section **section_p, struct frag **frag_p);

struct frag *symbol_get_frag (struct symbol *symbol);
struct section *symbol_get_section (struct symbol *symbol);

struct symbol *symbol_create (const char *name, struct section *section, unsigned long value, struct frag *frag);
struct symbol *symbol_find (const char *name);
struct symbol *symbol_find_or_make (char *name, int scope);
struct symbol *symbol_label (char *start, char *caret, char *name);
struct symbol *symbol_make (const char *name);
struct symbol *symbol_temp_new_now (void);

int symbol_force_reloc (struct symbol *symbol);
int symbol_is_external (struct symbol *symbol);
int symbol_is_resolved (struct symbol *symbol);
int symbol_is_section_symbol (struct symbol *symbol);
int symbol_is_undefined (struct symbol *symbol);
int symbol_uses_other_symbol (struct symbol *symbol);
int symbol_uses_reloc_symbol (struct symbol *symbol);

unsigned long symbol_get_symbol_table_index (struct symbol *symbol);
uint64_t symbol_get_value (struct symbol *symbol);
uint64_t symbol_resolve_value (struct symbol *symbol);

void symbol_add_to_chain (struct symbol *symbol);
void symbol_set_frag (struct symbol *symbol, struct frag *frag);
void symbol_set_external (struct symbol *symbol);
void symbol_set_section (struct symbol *symbol, struct section *section);
void symbol_set_symbol_table_index (struct symbol *symbol, unsigned long index);
void symbol_set_value (struct symbol *symbol, uint64_t value);
void symbol_set_value_expression (struct symbol *symbol, struct expr *expr);

#endif      /* _SYMBOL_H */

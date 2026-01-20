/******************************************************************************
 * @file            macro.h
 *****************************************************************************/
#ifndef     _MACRO_H
#define     _MACRO_H

#include    "vector.h"

#define     MACRO_BUILTIN               0
#define     MACRO_USER                  1

struct macro {

    char *name, *value;
    int is_variadic, type;
    
    struct vector args;
    int nargs;

};

#include    "hashtab.h"

struct hashtab_name *find_macro (char *sname);
struct macro *get_macro (struct hashtab_name *key);

void add_macro (char *start, char **pp, int report_line);
void remove_macro (char *start, char **pp, int report_line);

char *process_macro (char *start, char **pp, struct macro *m);

#endif      /* _MACRO_H */

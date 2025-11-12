/******************************************************************************
 * @file            kwd.h
 *****************************************************************************/
#ifndef     _KWD_H
#define     _KWD_H

struct pseudo_op_entry {

    const char *name;
    void (*handler) (char *start, char **pp);

};

struct pseudo_op_entry *find_poe (char *name);
struct pseudo_op_entry *find_data_poe (char *name);

void install_pseudo_op_table (struct pseudo_op_entry *table);

#endif      /* _KWD_H */

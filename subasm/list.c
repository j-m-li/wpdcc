/******************************************************************************
 * @file            list.c
 *****************************************************************************/
#include    "lib.h"
#include    "list.h"

void list_append (struct list **list, void *data) {

    struct list *new = xmalloc (sizeof (*new));
    struct list *old = (*list);
    
    if (old) {
    
        new->next = old->next;
        old->next = new;
    
    } else {
        new->next = new;
    }
    
    new->data = data;
    *list = new;

}

unsigned int nlist (struct list *list) {

    unsigned int n = 0;
    
    if (list) {
    
        struct list *p = list;
        do { n++; } while ((p = p->next) != list);
    
    }
    
    return n;

}

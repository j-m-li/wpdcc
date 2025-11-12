/******************************************************************************
 * @file            list.h
 *****************************************************************************/
#ifndef     _LIST_H
#define     _LIST_H

struct list {

    struct list *next;
    void *data;

};

void list_append (struct list **list, void *data);

#endif      /* _LIST_H */

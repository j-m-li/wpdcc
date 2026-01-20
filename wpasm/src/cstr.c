/******************************************************************************
 * @file            cstr.c
 *****************************************************************************/
#include    <stdlib.h>
#include    <string.h>

#include    "cstr.h"

extern void *xrealloc (void *__ptr, unsigned int __size);

static void cstr_realloc (struct cstring *cstr, int new_size) {

    int size = cstr->size_allocated;
    
    if (size < 8) {
        size = 8;
    }
    
    while (size < new_size) {
        size *= 2;
    }
    
    cstr->data = xrealloc (cstr->data, size);
    cstr->size_allocated = size;

}

void cstr_ccat (struct cstring *cstr, int ch) {

    int size = cstr->size + 1;
    
    if (size > cstr->size_allocated) {
        cstr_realloc (cstr, size);
    }
    
    ((unsigned char *) cstr->data)[size - 1] = ch;
    cstr->size = size;

}

void cstr_cat (struct cstring *cstr, const char *str, int len) {

    int size;
    
    if (len <= 0) {
        len = strlen (str) + 1 + len;
    }
    
    size = cstr->size + len;
    
    if (size > cstr->size_allocated) {
        cstr_realloc (cstr, size);
    }
    
    memmove (((unsigned char *) cstr->data) + cstr->size, str, len);
    cstr->size = size;

}

void cstr_new (struct cstring *cstr) {
    memset (cstr, 0, sizeof (struct cstring));
}

void cstr_free (struct cstring *cstr) {

    free (cstr->data);
    cstr_new (cstr);

}

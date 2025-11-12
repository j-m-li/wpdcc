/******************************************************************************
 * @file            cstr.h
 *****************************************************************************/
#ifndef     _CSTR_H
#define     _CSTR_H

struct cstring {

    int size, size_allocated;
    void *data;

};

void cstr_ccat (struct cstring *cstr, int ch);
void cstr_cat (struct cstring *cstr, const char *str, int len);

void cstr_new (struct cstring *cstr);
void cstr_free (struct cstring *cstr);

#endif      /* _CSTR_H */

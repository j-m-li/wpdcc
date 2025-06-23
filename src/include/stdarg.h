/*
 *  Adapted from www.pdos.org
 */

typedef char * va_list;

#define va_start(ap, parmN) ap = (char *)&parmN + sizeof(parmN)
#define va_arg(ap, type) *(type *)(ap += sizeof(type), ap - sizeof(type))
#define va_end(ap) ap = (char *)0

/******************************************************************************
 * @file            stdint.h
 *****************************************************************************/
#ifndef     _STDINT_H_INCLUDED
#ifndef     _STDINT_H
#ifndef     _STDINT_H_

#define     _STDINT_H_INCLUDED
#define     _STDINT_H
#define     _STDINT_H_

#include    <limits.h>

/* Add all data types (even though we don't use them) as the project seems to fail to build on some systems. */
typedef     signed char                 int8_t;
typedef     unsigned char               uint8_t;

typedef     signed short                int16_t;
typedef     unsigned short              uint16_t;

#if     INT_MAX > 32767
typedef     signed int                  int32_t;
typedef     unsigned int                uint32_t;
#else
typedef     signed long                 int32_t;
typedef     unsigned long               uint32_t;
#endif

#ifndef     _INT64_T
#define     _INT64_T
#if     defined (NO_LONG_LONG) /*|| ULONG_MAX > 4294967295UL*/
typedef     signed long                 int64_t;
#else
typedef     signed long long            int64_t;
#endif
#endif      /* _INT64_T */

#ifndef     _UINT64_T
#define     _UINT64_T
#if     defined (NO_LONG_LONG) /*|| ULONG_MAX > 4294967295UL*/
typedef     unsigned long               uint64_t;
#else
typedef     unsigned long long          uint64_t;
#endif
#endif      /* _UINT64_T */

#endif      /* _STDINT_H_ */
#endif      /* _STDINT_H */
#endif      /* _STDINT_H_INCLUDED */


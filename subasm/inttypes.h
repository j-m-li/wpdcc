/******************************************************************************
 * @file            inttypes.h
 *****************************************************************************/
#include    <limits.h>

#if     defined (NO_LONG_LONG)
# define    I64_FMT                     "l"
#elif   defined (_MSVC_)
# define    I64_FMT                     "I64"
#elif   defined (__PRI_64_LENGTH_MODIFIER__)                                                        /* Mac */
# define    I64_FMT                     __PRI_64_LENGTH_MODIFIER__
#elif   (defined (SIZEOF_LONG) && SIZEOF_LONG >= 8) || ULONG_MAX > 4294967295UL
# define    I64_FMT                     "l"
#else
# define    I64_FMT                     "ll"
#endif

#define     PRId64                      I64_FMT"d"
#define     PRIi64                      I64_FMT"i"
#define     PRIo64                      I64_FMT"o"
#define     PRIu64                      I64_FMT"u"
#define     PRIx64                      I64_FMT"x"
#define     PRIX64                      I64_FMT"X"


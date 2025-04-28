//------------------------------------------------------------------------------
//
//  BASELINE.H - Internal header file for "baseline" definitions
//
//  Edit History:
//  -------------
//   4Jun95 (fpj) - Original creation.
//	03Aug00 (cey) - Create/move source to XOSSRC/INC
//
//------------------------------------------------------------------------------

// ++++
// This software is in the public domain.  It may be freely copied and used
// for whatever purpose you see fit, including commerical uses.  Anyone
// modifying this software may claim ownership of the modifications, but not
// the complete derived code.  It would be appreciated if the authors were
// told what this software is being used for, but this is not a requirement.

//   THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
//   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
//   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----

#ifndef _BASELINE_H_
#define _BASELINE_H_

#ifdef _MSC_VER
/* MS visual C++ */

#pragma pack(1)
#define _Packed __declspec(align(1)) 
#define far
#define XOSCALL long

typedef unsigned char  uchar;
typedef signed   char  schar;
typedef unsigned short ushort;
typedef signed   short sshort;
typedef unsigned int   uint;
typedef signed   int   sint;
typedef unsigned long  ulong;
typedef signed   long  slong;
typedef          long long llong;
typedef unsigned long long ullong;
typedef signed   long long sllong;

typedef uchar  ui8;
typedef schar  si8;
typedef ushort ui16;
typedef sshort si16;
typedef ulong  ui32;
typedef slong  si32;
typedef ullong ui64;
typedef sllong si64;

typedef char const cchar;

static void BREAK(void)
{
}

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef TRUE
  #define TRUE  1
#endif

#else // _MSC_VER
// Special stuff to make WATCOM C do the right things

#pragma pack (1);
#pragma off (check_stack);
#pragma on (unreferenced);
#pragma aux default "*" modify [EAX ECX EDX];

// Watcom C does not provide a way to create keywords that specify function
//   characteristics. It does, however, provide 7 different predefined keywords
//   that do this and allows the definitions each to be changed. We use these
//   (in an almost completely transparent fashion) as follows
//		__cdecl    - Not used
//		__fastcall - Not used
//		__fortran  - Not used
//		__pascal   - Not used
//		__stdcall  - Not used
//		__syscall  - Redefined to describe the standard XOS calling sequence.
//		__watcall  - The default Watcom calling convention. Not changed.

// All XOS functions, including SVCs, use the same calling convention as
//   follows:

#pragma aux __syscall "*" parm reverse routine [] value struct float \
		struct caller [] modify [EAX ECX EDX]
#define XOSCALL long __syscall

// XOSFNC is the same as XOSCALL except that the return type is not included

#define XOSFNC __syscall

// XOS SVCs currently preserve all registers but are still defined using
//   XOSCALL to allow future modification to use SYSCALL or SYSENTER which
//   cannot respect ECX or EDX.

#define __XOS__

typedef unsigned char  uchar;
typedef signed   char  schar;
typedef unsigned short ushort;
typedef signed   short sshort;
typedef unsigned int   uint;
typedef signed   int   sint;
typedef unsigned long  ulong;
typedef signed   long  slong;
typedef          long long llong;
typedef unsigned long long ullong;
typedef signed   long long sllong;

typedef uchar  ui8;
typedef schar  si8;
typedef ushort ui16;
typedef sshort si16;
typedef ulong  ui32;
typedef slong  si32;
typedef ullong ui64;
typedef sllong si64;

typedef char const cchar;

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef TRUE
  #define TRUE  1
#endif

#define __XOS_clock_t typedef int clock_t;
#define __XOS_gid_t   typedef int gid_t;
#define __XOS_off_t   typedef int off_t;
#define __XOS_pid_t   typedef ulong pid_t;


#if !defined(_SIZE_T_DEFINED) && !defined(_SIZE_T_DEFINED_)
  #define _SIZE_T_DEFINED
  #define _SIZE_T_DEFINED_
  typedef ulong  size_t;
  typedef size_t   _w_size_t;
#endif

#if !defined(_SSIZE_T_DEFINED) && !defined(_SSIZE_T_DEFINED_)
  #define _SSIZE_T_DEFINED
  #define _SSIZE_T_DEFINED_
  typedef long ssize_t;
#endif


#define __XOS_uid_t   typedef int uid_t;
#define __XOS_wint_t  typedef int wint_t;
#define __XOS_dev_t   typedef int dev_t;
#define __XOS_ino_t   typedef int ino_t;
#define __XOS_mode_t  typedef int mode_t;
#define __XOS_nlink_t typedef int nlink_t;

typedef char *va_list[1];

#ifndef __cplusplus
  typedef ushort wchar_t;
#endif

#ifndef _POSIX_SOURCE

#define INT3 BREAK()
void BREAK(void);
#pragma aux BREAK = "INT 3"

// Allow including program to override

#ifndef FD_SETSIZE
  #define FD_SETSIZE 256
#endif

typedef struct fd_set
{	unsigned char fd_bits [((FD_SETSIZE) + 7) / 8];
} fd_set;

#define FD_SET(n, p)    ((p)->fd_bits[(n) / 8] |= (1 << ((n) & 7)))
#define FD_CLR(n, p)	((p)->fd_bits[(n) / 8] &= ~(1 << ((n) & 7)))
#define FD_ISSET(n, p)	((p)->fd_bits[(n) / 8] & (1 << ((n) & 7)))
#define FD_ZERO(p)	memset ((void *)(p), 0, sizeof (*(p)))

#endif // _POXIX_SOURCE

#ifndef NULL
  #define _NULL ((void *) 0)
  #define NULL ((void *) 0)
#endif

/// #define _CLOCKS_PER_SEC 50	// 50 ticks per second

#endif // _MSC_VER
#endif  // _BASELINE_H_

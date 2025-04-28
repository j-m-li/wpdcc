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

#ifndef XOSTHREADS_H
#define XOSTHREADS_H

#include <_prolog.h>

typedef struct thddata
{   QAB  qab;			// QAB for IO operations
    long number;		// Thread number
    char state;
    char wkseq;			// Wake sequence number
    char xxx[2];
    long alhndl;		// Alarm handle
    long stkalc;		// Base offset for allocated stack space
    long ESP;			// Saved ESP
} THDDATA;

extern THDDATA *thdData;

XOSCALL thdCtlAlarm(long thdbase, long func, long handle, time_s datetime);

XOSCALL thdCtlCreate(long thdbase, long stksize, long datasize, void (* fnc)(),
		void *arglst, long argnum);

XOSCALL thdCtlInitialize(long maxnum, long vectbase, long membase, long stksize,
		long datasize);
#pragma aux thdCtlInitialize modify exact [EAX ECX EBX EDX EDI ESI];

XOSCALL thdCtlKill(long thdbase);

XOSCALL thdCtlRelease(void);

XOSCALL thdCtlSuspend(long *flags, llong time);

XOSCALL thdCtlTerminate(void);

XOSCALL thdCtlWait(llong time);

XOSCALL thdCtlWake(long thdbase);

XOSCALL thdIoClose(long handle, long cmdbits);

XOSCALL thdIoDelete(long cmdbits, char *name, void *parms);

XOSCALL thdIoDevChar(long handle, void *chars);

XOSCALL thdIoDevParm(long cmdbits, char *name, void *parms);

XOSCALL thdIoFunc(QAB *qab);

XOSCALL thdIoInBlock(long handle, char *buffer, long count);

XOSCALL thdIoInBlockP(long handle, char *buffer, long count, void *parms);

XOSCALL thdIoOpen(long cmdbits, char *name, void *parms);

XOSCALL thdIoOutBlock(long handle, char *buffer, long count);

XOSCALL thdIoOutBlockP(long handle, char *buffer, long count, void *parms);

XOSCALL thdIoOutString(long handle, char *buffer, long count);

XOSCALL thdIoOutStringP(long handle, char *buffer, long count, void *parms);

XOSCALL thdIoRename(long cmdbits, char *newname, char *oldname, void *parms);

XOSCALL thdIoSpecial(long handle, long func, char *buffer, long count,
		void *parms);

# include <_epilog.h>

#endif

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

typedef struct fbrdata
{   QAB  qab;			// QAB for IO operations
    long number;		// Thread number
    char state;
    char wkseq;			// Wake sequence number
    char xxx[2];
    long alhndl;		// Alarm handle
    long stkalc;		// Base offset for allocated stack space
    long ESP;			// Saved ESP
} FBRDATA;

extern FBRDATA *fbrData;

XOSCALL fbrCtlAlarm(long fbrbase, long func, long handle, time_s datetime);

XOSCALL fbrCtlCreate(long fbrbase, long stksize, long datasize, void (* fnc)(),
		void *arglst, long argnum);

XOSCALL fbrCtlInitialize(long maxnum, long vectbase, long membase, long stksize,
		long datasize);
#pragma aux fbrCtlInitialize modify exact [EAX ECX EBX EDX EDI ESI];

XOSCALL fbrCtlKill(long fbrbase);

XOSCALL fbrCtlRelease(void);

XOSCALL fbrCtlSuspend(long *flags, llong time);

XOSCALL fbrCtlTerminate(void);

XOSCALL fbrCtlWait(llong time);

XOSCALL fbrCtlWake(long fbrbase);

XOSCALL fbrIoClose(long handle, long cmdbits);

XOSCALL fbrIoDelete(long cmdbits, char *name, void *parms);

XOSCALL fbrIoDevChar(long handle, void *chars);

XOSCALL fbrIoDevParm(long cmdbits, char *name, void *parms);

XOSCALL fbrIoFunc(QAB *qab);

XOSCALL fbrIoInBlock(long handle, char *buffer, long count);

XOSCALL fbrIoInBlockP(long handle, char *buffer, long count, void *parms);

XOSCALL fbrIoOpen(long cmdbits, char *name, void *parms);

XOSCALL fbrIoOutBlock(long handle, char *buffer, long count);

XOSCALL fbrIoOutBlockP(long handle, char *buffer, long count, void *parms);

XOSCALL fbrIoOutString(long handle, char *buffer, long count);

XOSCALL fbrIoOutStringP(long handle, char *buffer, long count, void *parms);

XOSCALL fbrIoRename(long cmdbits, char *newname, char *oldname, void *parms);

XOSCALL fbrIoSpecial(long handle, long func, char *buffer, long count,
		void *parms);

# include <_epilog.h>

#endif

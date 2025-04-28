//------------------------------------------------------------------------------
//
//  XOSSVC.H - Definitions for XOS system service calls
//
//  Edit History:
//  -------------
//  09/07/92(brn) - Add comment Header
//  13Apr95 (fpj) - Delete svc_xxx() prototypes.
//  20Jun95 (fpj) - Added prototypes for 16-bit code.
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

#ifndef _XOSSVC_H_
#define _XOSSVC_H_

//------------------------------------------------------------------------------
//
// This version of XOS only supports flat mode 32-bit system calls. The only
//   C compiler supported is owxc (which is the XOS port of OpenWatcom 1.8).
//
// Almost all exec and user mode system calls pass arguments on the stack from
//   right to left with the called function doing stack clean-up. The only
//   exceptions are a couple of functions with a variable number of arguments
//   which use the standard C calling sequence.
//
//------------------------------------------------------------------------------

# include <_prolog.h>

//  I/O SVCs

XOSCALL svcGphDone(long pid, long value, long bfraddr, long bfrsize,
		long regaddr, long regsize);
XOSCALL svcIoCancel(void *qab, long bits);
XOSCALL svcIoClear(long dev, long bits);
XOSCALL svcIoControl(void *qab, long func, long data);
XOSCALL svcIoDstName(char *rs, char *ms, char *ds, long dl);
XOSCALL svcIoDupHandle(long oldhndl, long newhndl, long cmdbits);
XOSCALL svcIoGetHndlMap(unsigned char *map, long size);
XOSCALL svcIoMakePipe(void);
XOSCALL svcIoPorts(long func, ushort *list);
XOSCALL svcIoQueue(void *qab);
XOSCALL svcIoTransName(long func, char *name, char *buffer, long size);
XOSCALL svcIoWait(void *pnt);

// Following are the obsolete names for the user mode IO functions

XOSCALL svcIoGetDosDevice(char *spec, char *buffer, long length);
XOSCALL svcIoClose(long dev, long bits);
XOSCALL svcIoClsChar(char *name, void *chrlst);
XOSCALL svcIoCommit(long dev);
XOSCALL svcIoDelete(long cmd, char *name, void *parm);
XOSCALL svcIoDevChar(long handle, void *chrlst);
XOSCALL svcIoDevParm(long bits, char *name, void *parm);
XOSCALL svcIoInBlock(long dev, char *data, long size);
XOSCALL svcIoInBlockP(long dev, char *data, long size, void *parm);
XOSCALL svcIoInSingle(long dev);
XOSCALL svcIoInSingleP(long dev, void *parm);
XOSCALL svcIoOpen(long bits, const char *name, void *parm);
XOSCALL svcIoOutBlock(long dev, char *data, long size);
XOSCALL svcIoOutBlockP(long dev, char *data, long size, void *parm);
XOSCALL svcIoOutSingle(long dev, long byte);
XOSCALL svcIoOutSingleP(long dev, long byte, void *parm);
XOSCALL svcIoOutString(long dev, char *data, long size);
XOSCALL svcIoOutStringP(long dev, char *data, long size, void *parm);
XOSCALL svcIoPath(long options, char *name, char *buffer, long length);
XOSCALL svcIoRename(long cmd, char *oname, char *nname, void *parm);
XOSCALL svcIoRun(void *qab);
XOSCALL svcIoSetPos(long dev, long pos, long mode);
XOSCALL svcIoSpecial(long dev, long func, char *addr, long count,
        void *parm);

// Following are the obsolete names for the user mode environment functions

XOSCALL svcSysFindEnv(cchar *name, cchar **found);
XOSCALL svcSysDefEnv(long level, cchar *str);
XOSCALL svcSysGetEnv(cchar *name, cchar **pstr);

// Logical name SVCs

XOSCALL svcLogDefine(long lvl, char *name, char *defin);
XOSCALL svcLogGet(long lvl, char *name, char *defin, long len);
XOSCALL svcLogGetAll(long lvl, char *buffer, long len);

// Memory management SVCs

XOSCALL svcMemBlkAlloc(long lapn, long size, long bits, long clientnum);
XOSCALL svcMemBlkChange(long lapn, long size, long bits, long dostask);
XOSCALL svcMemBlkFree(long dostask);
XOSCALL svcMemChange(void *base, long bits, long size);
XOSCALL svcMemAllocate(void *base, long bits, long size);
XOSCALL svcMemConvShr(void *addr, char *name, long type, long level,
        long prot, void *acl);
XOSCALL svcMemCreate(long select, long type);
XOSCALL svcMemDebug(long func, void *addr, long *value);
XOSCALL svcMemLink(long newselect, long oldselect, long type);
XOSCALL svcMemLinkShr(void *addr, char *name, long level);
XOSCALL svcMemMapPhys(void *base, long phys, long bits, long size);
XOSCALL svcMemMapSys(void *base, void *exec, long bits, long size);
XOSCALL svcMemMove(long nselect, long noffset, long oselect, long offset);
XOSCALL svcMemNull(char *base, long offset, long pagebits, long size);
XOSCALL svcMemPageType(void *base, long bottom, long top, long bits);
XOSCALL svcMemRemove(long select);
XOSCALL svcMemRmvMult(long data);
///XOSCALL svcMemV86SetUp(long size);
XOSCALL svcMemWPCntl(unsigned long mask);

XOSCALL svcMemWPSet(long number, long type, long size, void *address);

//  Scheduler svcs

XOSCALL svcSchClrAlarm(long handle);
XOSCALL svcSchSetAlarm(long vector, long data, llong dt);
XOSCALL svcSchClrEvent(char *name, long mask);
XOSCALL svcSchClrSignal(long vector);
XOSCALL svcSchCritDone(CRIT *crit);
XOSCALL svcSchCritWait(CRIT *crit);
XOSCALL svcSchCtlCDone(void);
XOSCALL svcSchExit(long status);
XOSCALL svcSchGetVector(long vector, void **addr);
XOSCALL svcSchInitVector(long type);
XOSCALL svcSchIntrProc(long pid, long func, long status);
///void    svcSchIRet(void);
XOSCALL svcSchKill(long status, long pid);
XOSCALL svcSchMakEvent(char *name, long size, long intr);
XOSCALL svcSchRelEvent(char *name, long mask);
XOSCALL svcSchReqSignal(long number, long *datapnt, long datasize, long pid);
XOSCALL svcSchResEvent(char *name, long event);
XOSCALL svcSchSetEvent(char *name, long event, long val, long pid);
XOSCALL svcSchSetLevel(long level);
XOSCALL svcSchSetVector(long vector, long lvl, void *addr);
XOSCALL svcSchSleep(void);
XOSCALL svcSchSuspend(long *flag, llong time);
XOSCALL svcSchWaitProc(long pid, long time);
XOSCALL svcSchWaitMEvent(char *name, long mask, long timeout);
XOSCALL svcSchWaitSEvent(char *name, long event, long timeout);
XOSCALL svcSchWaitSignal(long *flag, long time);
XOSCALL svcSchTerminate(long status, long tid);
XOSCALL	svcSchThread(long bits, long *stkpntr, long xxx, long (*thdfunc)(),
		long *args, long argnum);
XOSCALL svcSchWake(long tid);

//  Screen symbiont svcs

XOSCALL svcSssClsAlm(long exectdb, long func, long size, long pos, char *save);
XOSCALL svcSssDone(long exectdb, long clear[]);
XOSCALL svcSssGetMod(long exectdb, long *value);
XOSCALL svcSssGetTdb(long descriptor);
XOSCALL svcSssOpnAlm(long exectdb, long size);
XOSCALL svcSssPutAlm(long exectdb, long size, long pos, char *save, char *data);
XOSCALL svcSssRead(long exectdb, char *list[]);
XOSCALL svcSssSwitch(long oexectdb, long nexectdb, char *list[]);
XOSCALL svcSssWrite(long exectdb, char *list[]);

//  General system svcs

XOSCALL svcSysCmos(long addr, long data);
XOSCALL svcSysDateTime(long func, void *data);
XOSCALL svcSysErrMsg(long code, long format, char *str);
XOSCALL svcSysGetInfo(long func, long data, void *bfr, long size);
XOSCALL svcSysGetPData(long pid, void *data);
XOSCALL svcSysGetPid(void);
XOSCALL svcSysGetPLevel(long pid);
XOSCALL svcSysGetPName(long pid, void *str);
XOSCALL svcSysGetRandom(long mode, unsigned char *data, long size);
XOSCALL svcSysLoadLke(void *bfr);
XOSCALL svcSysLog(char *msg, long size);
XOSCALL svcSysSetPName(void *name);

//  Terminal svcs

XOSCALL svcTrmAttrib(long dev, long func, void *data);
XOSCALL svcTrmDebugIn(long dev, long dummy, long *data);
XOSCALL svcTrmCurPos(long dev, long page, long chr, long line);
XOSCALL svcTrmCurType(long dev, long curtype);
XOSCALL svcTrmDspMode(long dev, long func, void *data);
XOSCALL svcTrmDspPos(long dev, long pos);
XOSCALL svcTrmEgaPalet(long dev, void *list);
XOSCALL svcTrmFunction(long dev, long func);
XOSCALL svcTrmGetAtChr(long dev, long page);
XOSCALL svcTrmGetPixel(long dev, long page, long xcoord, long ycoord);
XOSCALL svcTrmLdCusFont(long dev, long table, long size, void *data,
		long begin, long count);
XOSCALL svcTrmLdStdFont(long dev, long table, long font, long begin,
		long count);
XOSCALL svcTrmMapScrn(long dev, void *buffer, long size, long offset);
XOSCALL svcTrmScroll(long dev, long page, long xul, long yul, long xlr,
        long ylr, long cnt, long attrib);
XOSCALL svcTrmSelFont(long descriptor, long fonta, long fontb);
XOSCALL svcTrmSetAtChr(long dev, long page, long chr, long attrib, long count);
XOSCALL svcTrmSetChr(long dev, long page, long chr, long color, long count);
XOSCALL svcTrmSetPixel(long dev, long page, long xcoord, long ycoord,
        long value);
XOSCALL svcTrmSwScreen(long dev, long num);
XOSCALL svcTrmWrtInB(long dev, cchar *str, long cnt);

// Following are system library functions - These mostly duplicate many of
//   the standard C library functions and can be executed in either user or
//   exec mode.

XOSCALL sysLibErrMsg(long code, long format, char *str);
XOSCALL sysLibStrNLen(cchar *str, long count);
XOSCALL sysLibStrLen(cchar *str);
XOSCALL sysLibStrChr(cchar *str, long chr);
XOSCALL sysLibStrMov(char *dst, cchar *src);
XOSCALL sysLibMemCpy(char *dst, cchar *src, long count);
XOSCALL sysLibMemRCpy(char *dst, cchar *src, long count);
XOSCALL sysLibMemCpyLong(char *dst, cchar *src, long count);
XOSCALL sysLibMemRCpyLong(char *dst, cchar *src, long count);
XOSCALL sysLibMemSetLong(char *addr, long value, long count);
XOSCALL sysLibErrMsg(long code, long bits, char *bufr);
long    sysLibSPrintF(char *bufr, cchar *fmt, ...);
XOSCALL sysLibVSPrintF(char *bufr, cchar *fmt, long *array);
/// XOSCALL sysLibStrICmp(char *str1, char *str2);
XOSCALL sysLibStrICmp(cchar *str1, cchar *str2);
XOSCALL sysLibStrNCmp(cchar *str1, cchar *str2, int len);
XOSCALL sysLibStrNICmp(cchar *str1, cchar *str2, int len);
XOSCALL sysLibStrRChr(cchar *str, int chr);
XOSCALL sysLibWildCmp8(long usecase, cchar *str1, cchar *str2);

// Following are functions for handling environment string

XOSCALL sysEnvDefine(long level, cchar *str);
XOSCALL sysEnvFind(cchar *name, cchar **found);
XOSCALL sysEnvGet(const char *name, cchar **pstr);
XOSCALL sysEnvGetAll(char *buffer, long length);

// Following are the new names for the user mode IO functions

XOSCALL sysIoGetDosDevice(char *spec, char *buffer, long length);
XOSCALL sysIoClose(long dev, long bits);
XOSCALL sysIoClsChar(char *name, char *chrlst);
XOSCALL sysIoCommit(long dev);
XOSCALL sysIoDelete(long cmd, char *name, void *parm);
XOSCALL sysIoDevChar(long handle, void *chrlst);
XOSCALL sysIoDevParm(long bits, char *name, void *parm);
XOSCALL sysIoNextFile(long dev, char *name, void *parm);
XOSCALL sysIoInBlock(long dev, char *data, long size);
XOSCALL sysIoInBlockP(long dev, char *data, long size, void *parm);
XOSCALL sysIoInSingle(long dev);
XOSCALL sysIoInSingleP(long dev, void *parm);
XOSCALL sysIoOpen(long bits, char *name, void *parm);
XOSCALL sysIoOutBlock(long dev, char *data, long size);
XOSCALL sysIoOutBlockP(long dev, char *data, long size, void *parm);
XOSCALL sysIoOutSingle(long dev, long byte);
XOSCALL sysIoOutSingleP(long dev, long byte, void *parm);
XOSCALL sysIoOutString(long dev, char *data, long size);
XOSCALL sysIoOutStringP(long dev, char *data, long size, void *parm);
XOSCALL sysIoPath(long options, char *name, char *buffer, long length);
XOSCALL sysIoRename(long cmd, char *oname, char *nname, void *parm);
XOSCALL sysIoRun(QAB *qab);
XOSCALL sysIoRunCmd(QAB *qab, char spec[], char *envname, char *extlist[]);
XOSCALL sysIoSetPos(long dev, long pos, long mode);
XOSCALL sysIoSpecial(long dev, long func, char *addr, long count, void *parm);

#include <_epilog.h>

#endif  // _XOSSVC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <xos.h>
#include <xosstr.h>
#include <xostime.h>
#include <xossvc.h>
#include <xoserrmsg.h>

/*
	.SBTTL	svcSysDateTime - Get or set system date and time

;Here for the svcSysDateTime SVC - Get or set system date and time
;  software interrupt level associated with fixed interrupts
;	long svcSysDateTime(
;	    int   function,	// Function:
;				//   1 = Get current date and time (XOS format)
;				//   2 = Get current date and time (DOS format)
;				//   3 = Get current date and time with time
;				//	   zone information (XOS format)
;				//   4 = Get current date and time with time
;				//	   zone information (DOS format)
;				//   5 = Reserved
;				//   6 = Get process elapsed time (XOS format)
;				//   7 = Get session elapsed time (XOS format)
;				//   8 = Get process CPU time (XOS format)
;				//   9 = Convert date/time to XOS format
;				//  10 = Convert date/time from XOS format
;				//  11 = Get current HR date and time (XOS
;				//	   format)
;				//  12 = Get process HR elapsed time (XOS
;				//	   format)
;				//  13 = Get session HR elapsed time (XOS
;				//	   format)
;				//  14 = Get current HR date and time with time
;				//	   zone information (XOS format)
;				//  15 = Get RTC date and time (DOS format)
;				//	   (requires admin priv)
;				//  16 = Set system date and time (XOS format)
;				//	   (requires admin priv)
;				//  17 = Set RTC date and time (DOS format)
;				//	   (requires admin priv)
;				//  18 = Get time zone information
;				//  19 = Set time zone information
;	    void *dttm);	// Address of argument structure (format depends
;				//   on function)
;  Value returned is 0 if normal or negative error code if error
*/
XOSCALL svcSysDateTime(long func, void *data)
{
	char *c = data;
	printf("svcSysDateTime %d\n", func);
	*c = 0;
	return 0;
}

XOSCALL svcSysGetEnv(cchar *name, cchar **pstr)
{
	*pstr = getenv(name);
	if (*pstr == NULL) return -1;
	return strlen(*pstr);
}

XOSCALL svcIoOpen(long bits, const char *name, void *parm)
{
	int f = O_BINARY;
	if (name == 0) return -1;
	if ((bits & (XO_IN|XO_OUT)) == (XO_IN|XO_OUT)) {
		f |= O_RDWR;
	} else if (bits & XO_IN) {
		f |= O_RDONLY;
	} else if (bits & XO_OUT) {
		f |= O_WRONLY;
	}
	if (bits & XO_TRUNCA) {
		f |= O_TRUNC;
	}
	if (bits & XO_CREATE) {
		f |= O_CREAT;
	}
	return _open(name, f, _S_IREAD | _S_IWRITE );
}

XOSCALL svcIoClose(long dev, long bits)
{
	return _close(dev);
}

XOSCALL svcIoInBlock(long dev, char far *data, long size)
{
	return _read(dev, data, size);
}

XOSCALL svcIoSetPos(long dev, long pos, long mode)
{
	return _lseek(dev, pos, SEEK_SET);
}

XOSCALL svcIoOutBlock(long dev, char *data, long size)
{
	return _write(dev, data, size);
}

XOSCALL svcMemChange(void *base, long bits, long size)
{
	printf("svcMemChange fixme\n");
//	free(base);
	return (char*)realloc(base, size) + size;
}

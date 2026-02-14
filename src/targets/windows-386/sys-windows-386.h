/*
 *	NMH's Simple C Compiler, 2011--2014
 *	Windows/386 (Linux cross compile) environment
 */

#define OS		"Windows-Cross"
#define AOUTNAME	"aout.exe"
#define ASCMD		"i686-w64-mingw32-as -g -o %s %s"
#define LDCMD		"i686-w64-mingw32-ld  --subsystem console -e _mainCRTStartup -o %s %s/lib/%scrt0.o -L /usr/i686-w64-mingw32/sys-root/mingw/lib/ -l user32 -l kernel32 -l gdi32"

#define SYSLIBC		""


#define DEFINES         "\0" /*"__SUBC__\0"*/


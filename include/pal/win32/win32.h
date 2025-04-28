/* Platform Abstaction Layer: MS Windows 32 bit */

#ifndef PAL__WIN32_H
#define PAL__WIN32_H


#include <lang/Object.h>

#pragma pack(8)
//#define IMPORT __declspec(dllimport)
#define IMPORT extern

#define EXCEPTION_EXECUTE_HANDLER       1
#define STD_INPUT_HANDLE    ((unsigned int)-10)
#define STD_OUTPUT_HANDLE   ((unsigned int)-11)
#define STD_ERROR_HANDLE    ((unsigned int)-12)
IMPORT void *__stdcall GetStdHandle(unsigned int nStdHandle);


IMPORT void *__stdcall CreateFileW(
	unsigned short *lpFileName,
    	unsigned int dwDesiredAccess,
    unsigned int dwShareMode,
   void * lpSecurityAttributes,
    unsigned int dwCreationDisposition,
   unsigned int dwFlagsAndAttributes,
    void * hTemplateFile
    );

IMPORT int __stdcall CloseHandle(void *handle);

IMPORT int __stdcall WriteFile(
    void *hFile,
    void * lpBuffer,
    unsigned int nNumberOfBytesToWrite,
   unsigned int *lpNumberOfBytesWritten,
    void *lpOverlapped
    );

IMPORT unsigned int __stdcall  GetFullPathNameW(
   unsigned short *lpFileName,
    unsigned int  nBufferLength,
    unsigned short *lpBuffer,
    unsigned short *lpFilePart
    );

IMPORT int __stdcall MultiByteToWideChar(
  	unsigned int     CodePage,
    	unsigned int    dwFlags,
    	char *   lpMultiByteStr,
   	int      cbMultiByte,
    	unsigned short *  lpWideCharStr,
    	int      cchWideChar);

IMPORT int __stdcall WideCharToMultiByte(
    unsigned int     CodePage,
    unsigned int    dwFlags,
    unsigned short *lpWideCharStr,
    int      cchWideChar,
    char *lpMultiByteStr,
    int      cbMultiByte,
    char *lpDefaultChar,
    int *lpUsedDefaultChar);


IMPORT int __stdcall  SetEndOfFile(
    void *hFile);

#define FILE_BEGIN 0
#define INVALID_SET_FILE_POINTER ((unsigned int)-1)
#define INVALID_HANDLE_VALUE ((void *)(long int *)-1)
unsigned int __stdcall	SetFilePointer(void * hFile, 
			long int lDistanceToMove,
				long int *lpDistanceToMoveHigh, 
				unsigned int dwMoveMethod);
#define NULL 0
#define NO_ERROR 0
#define CP_UTF8                   65001    
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5
#define FILE_SHARE_READ		(0x00000001)
#define FILE_SHARE_WRITE	(0x00000002)
#define	FILE_ATTRIBUTE_TEMPORARY		0x00000100
#define FILE_ATTRIBUTE_HIDDEN           0x00000002
#define FILE_ATTRIBUTE_NORMAL           0x00000080
#define FILE_FLAG_RANDOM_ACCESS     0x10000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000

int __stdcall DeleteFileA(const char * lpFileName);

unsigned int __stdcall GetCurrentDirectoryA(
		unsigned int nBufferLength, char * lpBuffer);

int __stdcall SetEnvironmentVariableA(
  char *lpName, char *lpValue);

IMPORT void * __stdcall GetProcAddress (
    void * hModule,
    char *lpProcName
    );

IMPORT void * __stdcall GetModuleHandleA(
    char *lpModuleName
    );

IMPORT void __stdcall RaiseException(
    unsigned int dwExceptionCode,
    unsigned int dwExceptionFlags,
   unsigned int nNumberOfArguments,
   unsigned long **lpArguments
    );

IMPORT unsigned int __stdcall GetLastError(void);
IMPORT void __stdcall Sleep(
    unsigned int dwMilliseconds
    );

IMPORT int __stdcall ReadFile(
    void *hFile,
    void *lpBuffer,
    unsigned int nNumberOfBytesToRead,
    unsigned int *lpNumberOfBytesRead,
    void *lpOverlapped
    );

void _CrtDbgBreak( void );

typedef char *  va_list;
#define _ADDRESSOF(v)   ( &(v) )
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define va_start(ap,v)  ( ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )

extern void system(char*);


EXPORT uint32 __stdcall GetEnvironmentVariableU(utf8 *lpName, utf8 *lpBuffer,
	uint32 nSize);

#define INVALID_FILE_ATTRIBUTES ((uint32)-1)
EXPORT uint32 __stdcall GetFileAttributesU(utf8 *lpName);

EXPORT utf8 *__stdcall GetCommandLineU(void);

typedef struct {  
	uint16 wYear;
	uint16 wMonth;
	uint16 wDayOfWeek;
	uint16 wDay;
	uint16 wHour;
	uint16 wMinute;
	uint16 wSecond;
	uint16 wMilliseconds;
} SYSTEMTIME;

IMPORT void __stdcall GetSystemTime(SYSTEMTIME *lpSystemTime);

#define LOCALE_USER_DEFAULT 0x0400
#define LOCALE_INVARIANT 0x007f

EXPORT int32 __stdcall GetDateFormatU(
  int32 Locale,               
  uint32 dwFlags,             
  const SYSTEMTIME *lpDate,  
  const utf8 *lpFormat,          
  utf8 *lpDateStr,          
  int32 cchDate               
);

EXPORT int32 __stdcall GetTimeFormatU(
  int32 Locale,              
  uint32 dwFlags,           
  const SYSTEMTIME *lpTime, 
  const utf8 *lpFormat,        
  utf8 *lpTimeStr,         
  int32 cchTime               
);

int32 win32_find_close(void *hFindFile);
void *win32_find_first_file(const utf8 *lpName, utf8 **first);
int32 win32_find_next_file(void *hFindFile, utf8 **first);
void *win32_wait_for_process(void *handle, uint32 t);
void *win32_close_process(void *handle, void *thread);
void *win32_create_process(utf8 *cmd, void **handle, void **thread);
#undef __declspec 
#endif // PAL__WIN32_H

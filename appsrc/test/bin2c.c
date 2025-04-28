#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define BUF_SIZE 80

HANDLE hFile, out; 

void __cdecl _tmain(int argc, TCHAR *argv[])
{
    DWORD dwBytesRead;
    char buf[BUF_SIZE];
    int i;
	DWORD dwResult;

    printf("\n");
    if( argc != 3 )
    {
        printf("ERROR:\tIncorrect number of arguments\n\n");
        printf("%s <file_name>\n", argv[0]);
        return;
    }

    hFile = CreateFile(argv[1],               // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no attr. template
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("Could not open file (error %d)\n", GetLastError());
        return; 
    }

    out = CreateFile(argv[2],                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_ALWAYS,          // overwrite existing
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

    if (out == INVALID_HANDLE_VALUE) 
    { 
        printf("Could not open file (error %d)\n", GetLastError());
        return;
    }

    i = 0;
    WriteFile (out, "{ ", 2, &dwResult, NULL);

    while(ReadFile(hFile, buf, 1, &dwBytesRead, NULL) && dwBytesRead != 0)
    {
	  

	if (i != 0 && i % 8 == 0) {
		WriteFile (out, ",\r\n", 3, &dwResult, NULL);
	} else if (i != 0) {
		WriteFile (out, ", ", 2, &dwResult, NULL);
	}

	if ((buf[0] >= 'a' && buf[0] <= 'z') ||
			(buf[0] >= 'A' && buf[0] <= 'Z') ||
			(buf[0] >= '0' && buf[0] <= '9'))
	{
		WriteFile (out, "'", 1, &dwResult, NULL);
		WriteFile (out, buf, 1, &dwResult, NULL);
		WriteFile (out, "'", 1, &dwResult, NULL);
	} else {
		char *h = "0123456789ABCDEF";
		WriteFile (out, "0x", 2, &dwResult, NULL);
		buf[1] = h[(buf[0] >> 4) & 0xf];
		buf[2] = h[buf[0] & 0xf];
		WriteFile (out, buf + 1, 2, &dwResult, NULL);
	}
	i++;
    }
WriteFile (out, "};", 2, &dwResult, NULL);
    CloseHandle(out);
    CloseHandle(hFile);
}


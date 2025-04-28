/* Platform Abstraction Layer */

#include <windows.h>
#include <lang/Object.h>

/* FIXME : these functions must be thread safe !!!! */

static wchar_t *get_utf16_buffer1(int32 size)
{
	static wchar_t *buf = 0;
	static int32 alloced = 0;

	if (size > alloced) {
		if (buf != 0) {
			HeapFree(GetProcessHeap(), 0, buf);
		}
		buf = HeapAlloc(GetProcessHeap(), 0, 
			(size + 1) * sizeof(wchar_t));
		if (buf == 0) {
			alloced = 0;
		} else {
			alloced = size;
		}
	}
	return buf;
}

static wchar_t *get_utf16_buffer2(int32 size)
{
	static wchar_t *buf = 0;
	static int32 alloced = 0;

	if (size > alloced) {
		if (buf != 0) {
			HeapFree(GetProcessHeap(), 0, buf);
		}
		buf = HeapAlloc(GetProcessHeap(), 0, 
			(size + 1) * sizeof(wchar_t));
		if (buf == 0) {
			alloced = 0;
		} else {
			alloced = size;
		}
	}
	return buf;
}

static utf8 *get_utf8_buffer(int32 size)
{
	static utf8 *buf = 0;
	static int32 alloced = 0;

	if (size > alloced) {
		if (buf != 0) {
			HeapFree(GetProcessHeap(), 0, buf);
		}
		buf = HeapAlloc(GetProcessHeap(), 0, 
			(size + 1) * sizeof(utf8));
		if (buf == 0) {
			alloced = 0;
		} else {
			alloced = size;
		}
	}
	return buf;
}
 
EXPORT uint32 __stdcall GetEnvironmentVariableU(utf8 *lpName, utf8 *lpBuffer,
	uint32 nSize)
{
	int32 l = 0;
	int32 nchar;
	uint32 r;
	wchar_t *wname;
	wchar_t *wbuf;

	while (lpName[l]) l++;
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, NULL, 0);
	wname = get_utf16_buffer1(nchar);
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, wname, nchar);
	wname[nchar] = 0;

	l = nSize / 4;
	wbuf = get_utf16_buffer2(l);
	r = GetEnvironmentVariableW(wname, wbuf, l);
	if (r >= (uint32)l) return r * 4;

	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, 0, 0, 0, 0);
	if ((uint32)l + 1 > nSize) return l + 1;
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, (LPSTR)lpBuffer, nSize, 0, 0);
	lpBuffer[l] = 0;
	return l;
}

EXPORT uint32 __stdcall GetFileAttributesU(utf8 *lpName)
{
	int32 l = 0;
	int32 nchar;
	wchar_t *wname;

	while (lpName[l]) l++;
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, NULL, 0);
	wname = get_utf16_buffer1(nchar);
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, wname, nchar);
	wname[nchar] = 0;

	return GetFileAttributesW(wname);
}

EXPORT utf8 *__stdcall GetCommandLineU(void)
{
	wchar_t *wbuf;
	int32 r = 0;
	int32 l;
	utf8 *lpBuffer;

	wbuf = GetCommandLineW();
	while (wbuf[r] != 0) r++;	
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, 0, 0, 0, 0);
	l++;
	lpBuffer = get_utf8_buffer(l);
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, (LPSTR)lpBuffer, l, 0, 0);
	lpBuffer[l] = 0;
	return lpBuffer;
}

EXPORT int32 __stdcall GetDateFormatU(
  int32 Locale,               
  uint32 dwFlags,             
  const SYSTEMTIME *lpDate,  
  const utf8 *lpFormat,          
  utf8 *lpDateStr,          
  int32 cchDate)
{
// TODO
	return GetDateFormatA(Locale, dwFlags, lpDate, (LPCSTR)lpFormat, 
		(LPSTR)lpDateStr, cchDate);
}

EXPORT int32 __stdcall GetTimeFormatU(
  int32 Locale,              
  uint32 dwFlags,           
  const SYSTEMTIME *lpTime, 
  const utf8 *lpFormat,        
  utf8 *lpTimeStr,         
  int32 cchTime)
{
// TODO
	return GetTimeFormatA(Locale, dwFlags, lpTime, (LPCSTR)lpFormat, 
		(LPSTR)lpTimeStr, cchTime);
}

void *win32_find_first_file(const utf8 *lpName, utf8 **first)
{
	WIN32_FIND_DATAW FindFileData;
	void *h;
	int32 hk;
	int32 l = 0;
	int32 r = 0;
	int32 nchar;
	wchar_t *wname;
	utf8 *lpBuffer;
	wchar_t *wbuf;

	while (lpName[l]) l++;
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, NULL, 0);
	wname = get_utf16_buffer1(nchar);
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, wname, nchar);
	wname[nchar] = 0;
	h = FindFirstFileW(wname, &FindFileData);
	if (h == INVALID_HANDLE_VALUE) {
		*first = 0;
		return (void*)-1;
	}
	hk = 1;
	while (hk != 0 && (FindFileData.cFileName[0] == L'.' &&
		(FindFileData.cFileName[1] == 0 || 
			(FindFileData.cFileName[1] == L'.' &&
				FindFileData.cFileName[2] == 0))))
	{
		hk = FindNextFileW(h, &FindFileData);
	}
	if (hk == 0) {
		*first = 0;
		return h;
	}
	wbuf = FindFileData.cFileName;
	while (wbuf[r] != 0) r++;	
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, 0, 0, 0, 0);
	l++;
	lpBuffer = get_utf8_buffer(l + 4);
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, (LPSTR)lpBuffer, l, 0, 0);
	if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		lpBuffer[l++] = '/';
	}
	lpBuffer[l] = 0;
	*first = lpBuffer; 
	return h;
}

int32 win32_find_next_file(void *hFindFile, utf8 **first)
{
	WIN32_FIND_DATAW FindFileData;
	int32 h;
	int32 l = 0;
	int32 r = 0;
	utf8 *lpBuffer;
	wchar_t *wbuf;

	h = FindNextFileW(hFindFile, &FindFileData);
	while (h != 0 && (FindFileData.cFileName[0] == L'.' &&
		(FindFileData.cFileName[1] == 0 || 
			(FindFileData.cFileName[1] == L'.' &&
				FindFileData.cFileName[2] == 0))))
	{
		h = FindNextFileW(hFindFile, &FindFileData);
	}
	if (h == 0) {
		*first = 0;
		return 0;
	}
	wbuf = FindFileData.cFileName;
	while (wbuf[r] != 0) r++;	
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, 0, 0, 0, 0);
	l++;
	lpBuffer = get_utf8_buffer(l + 4);
	l = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, (LPSTR)lpBuffer, l, 0, 0);
	if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		lpBuffer[l++] = '/';
	}
	lpBuffer[l] = 0;
	*first = lpBuffer; 
	return h;
}

int32 win32_find_close(void *hFindFile)
{
	return FindClose(hFindFile);
}

void win32_create_process(utf8 *lpName, void **handle, void **thread)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	int32 l = 0;
	int32 nchar;
	wchar_t *wname;

	*handle = INVALID_HANDLE_VALUE;
	*thread = INVALID_HANDLE_VALUE;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	while (lpName[l]) l++;
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, NULL, 0);
	wname = get_utf16_buffer1(nchar);
	nchar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpName, l, wname, nchar);
	wname[nchar] = 0;

	// Start the child process. 
	if (!CreateProcess( NULL,   // No module name (use command line)
		wname,          // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
	{
		return;
	}
	// Close process and thread handles. 
	*handle = pi.hProcess;
	*thread = pi.hThread;
}

void win32_wait_for_process(void *handle, uint32 t)
{
	WaitForSingleObject(handle, t);
}

void win32_close_process(void *handle, void *thread)
{
	CloseHandle(handle);
	CloseHandle(thread);
}

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);

typedef struct MyData {
	int val1;
	int val2;
} MYDATA, *PMYDATA;

int _tmain()
{
	PMYDATA pData;
	DWORD dwThread;
	HANDLE hThread;

	pData = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA));

	if(pData==NULL)
		ExitProcess(2);

	pData->val1=0;
	pData->val1=9;

	hThread = CreateThread(NULL,0,MyThreadFunction,pData,0,&dwThread);

	if(hThread==NULL) {
		ErrorHandler(TEXT("CreateThread"));
		ExitProcess(3);
	}

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	HeapFree(GetProcessHeap(), 0, pData);
	pData=NULL;

	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
	HANDLE hStdout;
	PMYDATA pData;
	TCHAR msgBuf[BUF_SIZE];
	size_t cchStringSize;
	DWORD dwChars;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hStdout==INVALID_HANDLE_VALUE)
		return 1;

	pData=(PMYDATA)lpParam;
	StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d,%d\n"), pData->val1, pData->val2);
	StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
	WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

	return 0;
}

void ErrorHandler(LPTSTR lpszFunction) {
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw=GetLastError();

	FormatMessage (
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM|
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,(lstrlen((LPCTSTR) lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf)/sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	 MessageBox(NULL, (LPCTSTR) lpDisplayBuf,TEXT("Error"),MB_OK);

	 LocalFree(lpMsgBuf);
	 LocalFree(lpDisplayBuf);
}


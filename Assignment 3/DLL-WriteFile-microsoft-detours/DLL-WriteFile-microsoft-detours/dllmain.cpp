// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "dllMain.h"


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	wstring path = L"C:\\temp\\" + to_wstring(GetCurrentProcessId()) + L".log";

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		originalWriteFile = WriteFile;
		hLogFile = CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)originalWriteFile, newWriteFile);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)originalWriteFile, newWriteFile);
		DetourTransactionCommit();
		break;

	}
	return TRUE;
}

BOOL WINAPI newWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	#define BUFSIZE 512
	WCHAR pszFilename[MAX_PATH + 1];
	wstring buffer = L" Write to file: ";
	DWORD numBytes;
	SYSTEMTIME st;
	GetSystemTime(&st);
	WCHAR currTime[200];
	swprintf(currTime, BUFSIZE, L"  Time: %d/%d/%d %d:%02d:%02d %d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute,st.wSecond, st.wMilliseconds);

	if (GetFinalPathNameByHandle(hFile, pszFilename, BUFSIZE, FILE_NAME_NORMALIZED))
	 buffer +=  wstring(pszFilename) ;
	else
		 buffer += L" Write to file: unknown";
	buffer += wstring(currTime) + L"\r\n\r\n";
	originalWriteFile(hLogFile, buffer.c_str(), buffer.size() * sizeof buffer[0], &numBytes, NULL);

	return originalWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

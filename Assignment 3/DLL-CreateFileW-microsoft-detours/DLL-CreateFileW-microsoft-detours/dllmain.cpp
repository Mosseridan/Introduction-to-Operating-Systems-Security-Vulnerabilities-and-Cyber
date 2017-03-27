// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "dllMain.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	wstring path = L"C:\\temp\\" + to_wstring(GetCurrentProcessId()) + L".log";

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		originalCreateFile = CreateFileW;
		hLogFile = originalCreateFile(path.c_str(), GENERIC_WRITE , FILE_SHARE_READ , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)originalCreateFile, newCreateFile);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)originalCreateFile, newCreateFile);
		DetourTransactionCommit();
		break;
		
	}
	return TRUE;
}

HANDLE WINAPI newCreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	DWORD numBytes;
//	DWORD dwMoved = ::SetFilePointer(hLogFile, 0l, nullptr, FILE_END );
	wstring buffer = wstring(lpFileName) + L"\r\n\r\n";
	WriteFile(hLogFile, buffer.c_str(), buffer.size() * sizeof buffer[0], &numBytes, NULL);

	return originalCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

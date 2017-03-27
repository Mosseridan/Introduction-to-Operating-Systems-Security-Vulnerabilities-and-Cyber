// proccess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include<string>

//getting pid and try to inject the dll
void injectDllOnProccess(DWORD processID)
{
	
	// Get a handle to the process. with request to run threads and memory allocation
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE, processID);
		
	if (NULL != hProcess)
	{	
		HMODULE hMod;
		DWORD cbNeeded;

		char* DirPath = new char[MAX_PATH];
		char* pathD = new char[MAX_PATH];

		//current direcory to contancate the address
		GetCurrentDirectoryA(MAX_PATH, DirPath);
		sprintf_s(pathD, MAX_PATH, "%s\\DLL-CreateFileW-microsoft-detours.dll", DirPath);

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA"); //addr for the thread

			LPVOID LLparam = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(pathD)+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
		
			WriteProcessMemory(hProcess, LLparam, pathD ,strlen(pathD)+1, NULL);

			HANDLE threadH = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibAddr, LLparam, NULL, NULL);
			if (!threadH)
				printf("error:%d", GetLastError());
			else
				CloseHandle(threadH);
		}
	}
	CloseHandle(hProcess);
}

int main()
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}

	// Calculate how many p	rocess identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	//pass on all proccess by pid
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
			injectDllOnProccess(aProcesses[i]);
	}

    return 0;
}


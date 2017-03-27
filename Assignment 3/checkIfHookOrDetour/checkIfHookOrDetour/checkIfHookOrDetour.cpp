// checkIfHookOrDetour.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	GetCurrentProcessId();
	IATcheck(GetModuleHandle(NULL));
    return 0;
}



bool IATcheck(HMODULE hInstance)
{
	bool flag = false;

	PIMAGE_IMPORT_DESCRIPTOR importedModule;
	PIMAGE_THUNK_DATA pFirstThunk, pOriginalFirstThunk;
	PIMAGE_IMPORT_BY_NAME pFuncData;

	importedModule = getImportTable(hInstance);
	while (*(WORD*)importedModule != 0) //over on the modules (DLLs)
	{
		pFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hInstance + importedModule->FirstThunk);//pointing to its IAT
		pOriginalFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hInstance + importedModule->OriginalFirstThunk);//pointing to OriginalThunk
		pFuncData = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hInstance + pOriginalFirstThunk->u1.AddressOfData);// and to IMAGE_IMPORT_BY_NAME
		while (*(WORD*)pFirstThunk != 0 && *(WORD*)pOriginalFirstThunk != 0) //moving over IAT and over names' table
		{
			printf("%X %s %x\n", pFirstThunk->u1.Function, pFuncData->Name, *(DWORD*)pFirstThunk->u1.Function);//printing function's name and addr
			
			if ((0x8b55ff8b != 	*(DWORD*)pFirstThunk->u1.Function))
				printf("function susepicious: %s \n", pFuncData->Name);//printing function's name and addr

			pOriginalFirstThunk++; // next node (function) in the array
			pFuncData = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hInstance + pOriginalFirstThunk->u1.AddressOfData);
			pFirstThunk++;// next node (function) in the array
		}
		importedModule++; //next module (DLL)
	}
	return false;
}

__declspec(naked) int newlstrcmpA()
{
	_asm {
		XOR eax, eax;
		RETN 8;
	}
}
/*
int WINAPI newlstrcmpA(LPCSTR a,LPCSTR b)
{
MessageBoxA(NULL,"hook called","hehe",MB_OK);
return 0;
}*/
PIMAGE_IMPORT_DESCRIPTOR getImportTable(HMODULE hInstance)
{
	PIMAGE_DOS_HEADER dosHeader;
	IMAGE_OPTIONAL_HEADER optionalHeader;
	PIMAGE_NT_HEADERS ntHeader;
	IMAGE_DATA_DIRECTORY dataDirectory;

	dosHeader = (PIMAGE_DOS_HEADER)hInstance;//cast hInstance to (IMAGE_DOS_HEADER *) - the MZ Header
	ntHeader = (PIMAGE_NT_HEADERS)((PBYTE)dosHeader + dosHeader->e_lfanew);//The PE Header begin after the MZ Header (which has size of e_lfanew)
	optionalHeader = (IMAGE_OPTIONAL_HEADER)(ntHeader->OptionalHeader); //Getting OptionalHeader
	dataDirectory = (IMAGE_DATA_DIRECTORY)(optionalHeader.DataDirectory[1]);//Getting the import table of DataDirectory
	return (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)hInstance + dataDirectory.VirtualAddress);//ImageBase+RVA to import table

}
bool rewriteThunk(PIMAGE_THUNK_DATA pThunk, void* newFunc)
{
	DWORD CurrentProtect;
	DWORD junk;
	VirtualProtect(pThunk, 4096, PAGE_READWRITE, &CurrentProtect);//allow write to the page
	if ((DWORD)newFunc == (DWORD)pThunk->u1.Function)
		return false;
	//originalCreateFile =  (HANDLE(WINAPI *)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)) pThunk->u1.Function;
	pThunk->u1.Function = (DWORD)newFunc; // rewrite the IAT to new function
	VirtualProtect(pThunk, 4096, CurrentProtect, &junk);//return previous premissions
	return true;
}



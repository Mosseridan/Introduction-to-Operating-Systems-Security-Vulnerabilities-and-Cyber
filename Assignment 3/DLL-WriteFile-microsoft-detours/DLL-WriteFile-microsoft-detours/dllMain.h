#pragma once
#pragma once
#include <windows.h>
#include<conio.h>
#include<iostream>
#include<string>
#include <direct.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>
#pragma comment(lib, "detours.lib")
#include "detours.h"

#define TARGET_FUNCTION "CreateFileW"
#define IMPORT_TABLE_OFFSET 1
using namespace std;
BOOL GetFileNameFromHandle(HANDLE hFile, TCHAR* pszFilename);
BOOL (WINAPI *originalWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
BOOL(WINAPI newWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
HANDLE hLogFile;
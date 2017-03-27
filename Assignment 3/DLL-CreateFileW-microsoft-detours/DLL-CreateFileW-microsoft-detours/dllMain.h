#pragma once
#include <windows.h>
#include<conio.h>
#include<iostream>
#include<string>
#include <direct.h>
#pragma comment(lib, "detours.lib")
#include "detours.h"

#define TARGET_FUNCTION "CreateFileW"
#define IMPORT_TABLE_OFFSET 1
using namespace std;

HANDLE(WINAPI *originalCreateFile)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
HANDLE(WINAPI newCreateFile)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
HANDLE hLogFile;
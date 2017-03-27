#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <windows.h>
#include<conio.h>
#include<iostream>
#include<string>
#include <SDKDDKVer.h>
bool hook(HMODULE, LPCSTR, PVOID);
bool rewriteThunk(PIMAGE_THUNK_DATA pThunk, void* newFunc);
PIMAGE_IMPORT_DESCRIPTOR getImportTable(HMODULE);
bool IATcheck(HMODULE hInstance);
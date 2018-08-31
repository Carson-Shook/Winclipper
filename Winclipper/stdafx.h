// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN64
#define AGNOSTIC_USERDATA		GWLP_USERDATA
#else
#define AGNOSTIC_USERDATA		GWL_USERDATA
#endif

#define MAX_LOADSTRING 100
#define ID_REG_HOTKEY 100

#include "targetver.h"
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#undef _WIN32_IE
#define _WIN32_IE 0x0600
#include <shellapi.h>
#include <CommCtrl.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <Shlwapi.h>
#include <string>
#include <thread>
#include <deque>
#include <vector>
#include <limits>
#include <map>

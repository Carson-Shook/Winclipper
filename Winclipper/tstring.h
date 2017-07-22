#pragma once
#include <string>
#ifndef UNICODE
#define TO_TSTRING std::to_string
typedef std::string TSTRING;
#else
#define TO_TSTRING std::to_wstring
typedef std::wstring TSTRING;
#endif

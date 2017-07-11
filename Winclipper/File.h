#pragma once
#ifndef UNICODE
#define TO_TSTRING std::to_string
typedef std::string TSTRING;
typedef std::ofstream TOFSTREAM;
typedef std::ifstream TIFSTREAM;
#define TSTRING_ITERATOR_ARGS std::string
#else
#define TO_TSTRING std::to_wstring
typedef std::wstring TSTRING;
typedef std::wofstream TOFSTREAM;
typedef std::wifstream TIFSTREAM;
#define TSTRING_ITERATOR_ARGS std::wstring, wchar_t
#endif


class File
{   
public:
    File();
    ~File();

    static bool Exists(const TCHAR* name);
    static TSTRING File::GetDirName(const TSTRING& fname);
    static void WriteAllLines(const TCHAR* name, std::vector<TSTRING> lines);
    static std::vector<TSTRING> ReadAllLines(const TCHAR* name);
};
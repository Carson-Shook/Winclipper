#pragma once
#ifndef UNICODE
typedef std::ofstream TOFSTREAM;
typedef std::ifstream TIFSTREAM;
#define TSTRING_ITERATOR_ARGS std::string
#else
typedef std::wofstream TOFSTREAM;
typedef std::wifstream TIFSTREAM;
#define TSTRING_ITERATOR_ARGS std::wstring, wchar_t
#endif

// Contains a collection of file utilities for easy file management.
class File
{   
private:
    File() { };
    ~File() { };

public:
    static bool Exists(const TCHAR* name);
    static TSTRING File::GetDirName(const TSTRING& fname);
    static void WriteAllLines(const TCHAR* name, std::vector<TSTRING> lines);
    static std::vector<TSTRING> ReadAllLines(const TCHAR* name);
};
#pragma once
#ifndef UNICODE
typedef std::string TSTRING;
#define TO_TSTRING std::to_string
#else
typedef std::wstring TSTRING;
#define TO_TSTRING std::to_wstring
#endif


class File
{
private:
    
public:
    File();
    ~File();

    static bool Exists(const TCHAR* name);
    static void WriteAllLines(const TCHAR* name, std::vector<std::string> lines);
};
#pragma once

// Contains a collection of file utilities for easy file management.
class File
{   
private:
    File() { };
    ~File() { };

public:
    static std::wstring File::GetDirName(const std::wstring& fname);
    static bool Exists(const wchar_t* name);
    static bool Delete(const wchar_t* name);
    static void WriteAllLines(const wchar_t* name, std::vector<std::wstring> lines);
    static std::vector<std::wstring> ReadAllLines(const wchar_t* name);
    static std::deque<wchar_t *> BinaryReadDeque(const wchar_t * name);
    static void BinaryWriteDeque(const std::deque<wchar_t *> data, const wchar_t * name);
};
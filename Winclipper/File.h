#pragma once
#include "stdafx.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <codecvt>
#include <fstream>
#include <sstream>
#include <iterator>
#include "base64.h"
#include "Bitmap.h"
#include "ISerializable.h"

// Contains a collection of file utilities for easy file management.
// For serailzation purposes, std::string is preferred because it makes
// working with Base64 encoded strings much easier. Filenames still use
// std::wstrings as they are used all throughout the other parts of
// this application. Older, less safe methods are maintained for
// serialization backwards-compatability.
class File
{   
public:
	static const char DIR_SEP = '\\';

    static std::wstring GetDirName(const std::wstring& fname);
	static std::wstring JoinPath(const std::wstring& path1, const std::wstring& path2);
	static const std::wstring GetAppDir();

    static bool Exists(const wchar_t* name) noexcept;
    static bool Delete(const wchar_t* name) noexcept;
	static void Write(std::wstring filename, std::string data);
	static std::string Read(std::wstring filename);
    static void WriteAllLines(const wchar_t* name, std::vector<std::wstring> lines);
    static std::vector<std::wstring> ReadAllLines(const wchar_t* name);

private:
	File() noexcept {};
	~File() {};

	static std::wstring appDir;
};

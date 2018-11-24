#pragma once
#include "stdafx.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <codecvt>
#include <fstream>
#include <iterator>
#include "base64.h"
#include "ISerializable.h"

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
	
	template <class T> static std::deque<T *> ReadAllObjects(const wchar_t * name)
	{
		std::ifstream inFile(name, std::ios::binary);
		if (!inFile) {
			throw std::runtime_error("Could not open file for reading");
		}

		std::deque<T *> retVal;

		while (!inFile.eof())
		{
			std::string inString;
			inFile >> inString;

			if (inString.compare("") == 0)
			{
				break;
			}

			T * pushVal = new T;
			dynamic_cast<ISerializable *>(pushVal)->Deserialize(inString);
			retVal.push_back(pushVal);
		}

		return retVal;
	}

	template <class T> static void WriteAllObjects(const std::deque<T *> objects, const wchar_t * name)
	{
		if (!File::Exists(name))
		{
			SHCreateDirectoryEx(nullptr, (GetDirName(name).c_str()), nullptr);
		}

		std::ofstream outFile(name, std::ios::binary);
		if (!outFile) {
			throw std::runtime_error("Could not open file for writing");
		}

		for (ISerializable * object : objects)
		{
			outFile << object->Serialize();
			outFile << "\r\n";
		}

		outFile.close();
	}

    static std::vector<std::wstring> ReadAllLines(const wchar_t* name);
    static std::deque<wchar_t *> BinaryReadDeque(const wchar_t * name);
    static void BinaryWriteDeque(const std::deque<wchar_t *> data, const wchar_t * name);
};
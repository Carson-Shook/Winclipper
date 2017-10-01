#include "stdafx.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <codecvt>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <deque>
#include "base64.h"
#include "File.h"

// Gets the directory path from a given file path.
TSTRING File::GetDirName(const TSTRING& fname)
{
    TSTRING directory;
    const size_t last_slash_idx = fname.rfind('\\');
    if (std::string::npos != last_slash_idx)
    {
        directory = fname.substr(0, last_slash_idx);
    }

    return directory;
}

// Determines whether or not a file eixists
bool File::Exists(const TCHAR *name)
{
    int retval = PathFileExists(name);
    if (retval == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Writes all lines in a vector to a text file
void File::WriteAllLines(const TCHAR* name, std::vector<TSTRING> lines)
{
    if (!File::Exists(name))
    {
        SHCreateDirectoryEx(NULL, (GetDirName(name).c_str()), NULL);
    }
    TOFSTREAM output_file(name);
    std::ostream_iterator<TSTRING_ITERATOR_ARGS> output_iterator(output_file, _T("\n"));
    std::copy(lines.begin(), lines.end(), output_iterator);
}

// Reads newline delimited values from a file into a vector
std::vector<TSTRING> File::ReadAllLines(const TCHAR * name)
{
    std::vector<TSTRING> lines;
    TIFSTREAM myfile(name);

    std::copy(std::istream_iterator<TSTRING_ITERATOR_ARGS>(myfile),
        std::istream_iterator<TSTRING_ITERATOR_ARGS>(),
        std::back_inserter(lines));
    return lines;
}

std::deque<TCHAR *> File::BinaryReadDeque(const TCHAR * name)
{
    std::ifstream inFile(name, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Could not open file for reading");
    }

    std::deque<TCHAR *> retVal;

    // Really need to go through all of this and consider what
    // does and doesn't need to be done to account for UNICODE.
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    while (!inFile.eof())
    {
        std::string base64str;
        inFile >> base64str;

        if (base64str.compare("") == 0)
        {
            break;
        }

        std::string byteStr = base64_decode(base64str);
        std::wstring wideStr = converter.from_bytes(byteStr);
        wchar_t * pushVal = _wcsdup(wideStr.c_str());
        retVal.push_back(pushVal);
    }

    return retVal;
}

void File::BinaryWriteDeque(const std::deque<TCHAR *> data, const TCHAR * name)
{
    if (!File::Exists(name))
    {
        SHCreateDirectoryEx(NULL, (GetDirName(name).c_str()), NULL);
    }

    std::ofstream outFile(name, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Could not open file for writing");
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    for (int i = 0, j = data.size(); i < j; i++)
    {
        std::string anotherTemp = converter.to_bytes(data.at(i));
        const char * temp = anotherTemp.c_str();
        outFile << base64_encode((unsigned char *)temp, strlen(temp) + 1);
        outFile << "\r\n";
    }

    outFile.close();
}

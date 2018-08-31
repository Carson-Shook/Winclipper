#include "stdafx.h"
#include "File.h"

// Gets the directory path from a given file path.
std::wstring File::GetDirName(const std::wstring& fname)
{
    std::wstring directory;
    const size_t last_slash_idx = fname.rfind('\\');
    if (std::string::npos != last_slash_idx)
    {
        directory = fname.substr(0, last_slash_idx);
    }

    return directory;
}

// Determines whether or not a file eixists
bool File::Exists(const wchar_t *name)
{
    int retval = PathFileExists(name);
    if (retval == 1)
    {
        return true;
    }
    return false;
}

bool File::Delete(const wchar_t * name)
{
    if (DeleteFile(name) == 0)
    {
        return true;
    }
    return false;
}

// Writes all lines in a vector to a text file
void File::WriteAllLines(const wchar_t* name, std::vector<std::wstring> lines)
{
    if (!File::Exists(name))
    {
        SHCreateDirectoryEx(NULL, (GetDirName(name).c_str()), NULL);
    }
    std::wofstream output_file(name);
    std::ostream_iterator<std::wstring, wchar_t> output_iterator(output_file, L"\n");
    std::copy(lines.begin(), lines.end(), output_iterator);
}

// Reads newline delimited values from a file into a vector
std::vector<std::wstring> File::ReadAllLines(const wchar_t * name)
{
    std::vector<std::wstring> lines;
    std::wifstream myfile(name);

    std::copy(std::istream_iterator<std::wstring, wchar_t>(myfile),
        std::istream_iterator<std::wstring, wchar_t>(),
        std::back_inserter(lines));
    return lines;
}

std::deque<wchar_t *> File::BinaryReadDeque(const wchar_t * name)
{
    std::ifstream inFile(name, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Could not open file for reading");
    }

    std::deque<wchar_t *> retVal;

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

void File::BinaryWriteDeque(const std::deque<wchar_t *> data, const wchar_t * name)
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

    for (size_t i = 0, j = data.size(); i < j; i++)
    {
        std::string anotherTemp = converter.to_bytes(data.at(i));
        const char * temp = anotherTemp.c_str();
        outFile << base64_encode((unsigned char *)temp, (unsigned int)strlen(temp) + 1);
        outFile << "\r\n";
    }

    outFile.close();
}

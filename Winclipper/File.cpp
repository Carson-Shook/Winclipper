#include "stdafx.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
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

#include "stdafx.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include "File.h"

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

std::vector<TSTRING> File::ReadAllLines(const TCHAR * name)
{
    std::vector<TSTRING> lines;
    TIFSTREAM myfile(name);

    std::copy(std::istream_iterator<TSTRING_ITERATOR_ARGS>(myfile),
        std::istream_iterator<TSTRING_ITERATOR_ARGS>(),
        std::back_inserter(lines));
    return lines;
}

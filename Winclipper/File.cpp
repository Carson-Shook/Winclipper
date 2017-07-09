#include "stdafx.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include "File.h"

TSTRING dirnameOf(const TSTRING& fname)
{
    TSTRING directory;
    const size_t last_slash_idx = fname.rfind('\\');
    if (std::string::npos != last_slash_idx)
    {
        directory = fname.substr(0, last_slash_idx);
    }

    return directory;
}

File::File()
{
    
}

File::~File()
{

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

void File::WriteAllLines(const TCHAR* name, std::vector<std::string> lines)
{
    if (!File::Exists(name))
    {
        SHCreateDirectoryEx(NULL, (dirnameOf(name).c_str()), NULL);
    }
    std::ofstream output_file(name);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(lines.begin(), lines.end(), output_iterator);
}

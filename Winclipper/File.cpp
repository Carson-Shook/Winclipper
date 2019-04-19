#include "stdafx.h"
#include "File.h"

// Gets the directory path from a given file path.
std::wstring File::GetDirName(const std::wstring& fname)
{
    std::wstring directory;
    const size_t sepIndex = fname.rfind('\\');
    if (std::wstring::npos != sepIndex)
    {
        directory = fname.substr(0, sepIndex);
    }

    return directory;
}

std::wstring File::JoinPath(const std::wstring & path1, const std::wstring & path2)
{
	auto retVal = path1;
	auto retVal2 = path2;
	if (retVal.back() != DIR_SEP)
	{
		retVal += DIR_SEP;
	}
	
	if (retVal2.front() == DIR_SEP)
	{
		retVal2.erase(0, 1);
	}

	return retVal + retVal2;
}

// Returns location of Winclipper's Local AppData directory
const std::wstring File::GetAppDir()
{
	if (appDir.empty())
	{
		wchar_t * tempClipsPath;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempClipsPath)))
		{
			// Ends with DIR_SEP since 99% of the time we're appending a filename.
			appDir = JoinPath(tempClipsPath, L"\\Winclipper\\Winclipper\\");
			CoTaskMemFree(tempClipsPath);
		}
		else
		{
			throw std::exception("Your AppData folder could not be found.");
		}
	}
	
	return appDir;
}

// Determines whether or not a file eixists
bool File::Exists(const wchar_t *name)
{
    int retval = PathFileExistsW(name);
    if (retval == 1)
    {
        return true;
    }
    return false;
}

bool File::Delete(const wchar_t * name)
{
    if (DeleteFileW(name) == 0)
    {
        return true;
    }
    return false;
}

void File::Write(std::wstring filename, std::string data)
{
	if (!File::Exists(filename.c_str()))
	{
		SHCreateDirectoryExW(NULL, (GetDirName(filename).c_str()), NULL);
	}
	std::ofstream outStream(filename, std::ios::out | std::ios::binary);

	if (outStream)
	{
		outStream << data;
		outStream.close();
	}
	// Add error handling
}

std::string File::Read(std::wstring filename)
{
	// See: http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
	std::ifstream inStream(filename, std::ios::in | std::ios::binary);
	if (inStream)
	{
		std::ostringstream contents;
		contents << inStream.rdbuf();
		inStream.close();
		return contents.str();
	}
	return std::string();
}

// Writes all lines in a vector to a text file
void File::WriteAllLines(const wchar_t* name, std::vector<std::wstring> lines)
{
    if (!File::Exists(name))
    {
        SHCreateDirectoryExW(NULL, (GetDirName(name).c_str()), NULL);
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

std::wstring File::appDir;

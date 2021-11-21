#include "stdafx.h"
#include <string>
#include "RegistryUtilities.h"

unsigned int RegistryUtilities::GetWindows10ReleaseId()
{
	unsigned int retVal = 0;
	HKEY hOpened;

	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &hOpened) == ERROR_SUCCESS)
	{
		if (QueryKeyForValue(hOpened, L"ReleaseId") == true)
		{
			const size_t bufferSize = 4096;
			wchar_t data[bufferSize];
			DWORD cbData = DWORD{ bufferSize };

			// Really should check if ERROR_MORE_DATA is returned, but in practice this
			// string should never be longer than 5 wchar_t, and if it ever is, we'll
			// probably find plenty of time to prepare.
			if (RegQueryValueExW(hOpened, L"ReleaseId", nullptr, nullptr, reinterpret_cast<LPBYTE>(&data), &cbData) == ERROR_SUCCESS)
			{
				try
				{
					retVal = std::stoi(data);
				}
				catch (const std::exception&)
				{
					retVal = 0;
				}
			}
		}
	}
	RegCloseKey(hOpened);

	return retVal;
}

bool RegistryUtilities::AppsUseLightTheme()
{
	bool retVal = true;
	HKEY hOpened;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_QUERY_VALUE, &hOpened) == ERROR_SUCCESS)
	{
		if (QueryKeyForValue(hOpened, L"AppsUseLightTheme") == true)
		{
			const size_t bufferSize = 4096;
			INT32 data;
			DWORD cbData = DWORD{ bufferSize };

			if (RegQueryValueExW(hOpened, L"AppsUseLightTheme", nullptr, nullptr, reinterpret_cast<LPBYTE>(&data), &cbData) == ERROR_SUCCESS)
			{
				try
				{
					retVal = data == 1;
				}
				catch (const std::exception&)
				{
					retVal = true;
				}
			}
		}
	}
	RegCloseKey(hOpened);

	return retVal;
}

bool RegistryUtilities::ClipboardHistoryEnabled()
{
	bool retVal = false;
	HKEY hOpened;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Clipboard", 0, KEY_QUERY_VALUE, &hOpened) == ERROR_SUCCESS)
	{
		if (QueryKeyForValue(hOpened, L"EnableClipboardHistory") == true)
		{
			const size_t bufferSize = 4096;
			INT32 data;
			DWORD cbData = DWORD{ bufferSize };

			if (RegQueryValueExW(hOpened, L"EnableClipboardHistory", nullptr, nullptr, reinterpret_cast<LPBYTE>(&data), &cbData) == ERROR_SUCCESS)
			{
				try
				{
					retVal = data == 1;
				}
				catch (const std::exception&)
				{
					retVal = false;
				}
			}
		}
	}
	RegCloseKey(hOpened);

	return retVal;
}

bool RegistryUtilities::DisableClipboardHistory()
{
	HKEY hOpened;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Clipboard", 0, KEY_ALL_ACCESS, &hOpened) == ERROR_SUCCESS)
	{
		DWORD lpData = 0;
		RegSetKeyValueW(hOpened, nullptr, L"EnableClipboardHistory", REG_DWORD, reinterpret_cast<LPBYTE>(&lpData), sizeof(lpData));
	}
	else
	{
		return false;
	}

	RegCloseKey(hOpened);
	return true;
}

bool RegistryUtilities::ClipboardStartupWarningDisabled()
{
	bool retVal = false;
	HKEY hOpened;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Winclipper", 0, KEY_QUERY_VALUE, &hOpened) == ERROR_SUCCESS)
	{
		if (QueryKeyForValue(hOpened, L"DisableStartupWarning") == true)
		{
			const size_t bufferSize = 4096;
			INT32 data;
			DWORD cbData = DWORD{ bufferSize };

			if (RegQueryValueExW(hOpened, L"DisableStartupWarning", nullptr, nullptr, reinterpret_cast<LPBYTE>(&data), &cbData) == ERROR_SUCCESS)
			{
				try
				{
					retVal = data == 1;
				}
				catch (const std::exception&)
				{
					retVal = false;
				}
			}
		}
	}
	RegCloseKey(hOpened);

	return retVal;
}

bool RegistryUtilities::DisableClipboardStartupWarning()
{
	HKEY hOpened;
	LPDWORD phkResult = nullptr;

	if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Winclipper", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hOpened, phkResult) == ERROR_SUCCESS)
	{
		DWORD lpData = 1;
		RegSetKeyValueW(hOpened, nullptr, L"DisableStartupWarning", REG_DWORD, reinterpret_cast<LPBYTE>(&lpData), sizeof(lpData));
	}
	else
	{
		return false;
	}

	RegCloseKey(hOpened);
	return true;
}

// Determines whether or not a particular value exists for a given key
bool RegistryUtilities::QueryKeyForValue(HKEY hKey, wchar_t* checkValue)
{
	wchar_t    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	std::unique_ptr<wchar_t> achValue(new wchar_t[MAX_REG_VALUE_NAME]);
	DWORD cchValue = MAX_REG_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKeyW(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

								 // Enumerate the key values. 

	if (cValues)
	{
		for (i = 0, retCode = ERROR_SUCCESS; i<cValues; i++)
		{
			DWORD cchValue = MAX_REG_VALUE_NAME;
			achValue.get()[0] = '\0';
			retCode = RegEnumValueW(hKey, i,
				achValue.get(),
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{
				if (wcscmp(checkValue, achValue.get()) == 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

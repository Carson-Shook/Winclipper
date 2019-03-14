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
			if (RegQueryValueExW(hOpened, L"ReleaseId", NULL, NULL, reinterpret_cast<LPBYTE>(&data), &cbData) == ERROR_SUCCESS)
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

	wchar_t * achValue;
	achValue = new wchar_t[MAX_REG_VALUE_NAME];
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
			cchValue = MAX_REG_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValueW(hKey, i,
				achValue,
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{
				if (wcscmp(checkValue, achValue) == 0)
				{
					return true;
				}
			}
		}
	}
	delete[] achValue;
	return false;
}

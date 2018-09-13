#include "stdafx.h"
#include "RegistryUtilities.h"

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

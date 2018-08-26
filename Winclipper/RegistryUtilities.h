#pragma once

#define MAX_REG_VALUE_NAME 16383

class RegistryUtilities
{
public:
	static bool QueryKeyForValue(HKEY, wchar_t*);
};
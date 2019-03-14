#pragma once

#define MAX_REG_VALUE_NAME 16383

class RegistryUtilities
{
public:
	static unsigned int GetWindows10ReleaseId();
	static bool QueryKeyForValue(HKEY, wchar_t*);
};
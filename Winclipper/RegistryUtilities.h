#pragma once

#define MAX_REG_VALUE_NAME 16383

class RegistryUtilities
{
public:
	static unsigned int GetWindows10ReleaseId();
	static bool AppsUseLightTheme();
	static bool ClipboardHistoryEnabled();
	static bool DisableClipboardHistory();
	static bool ClipboardStartupWarningDisabled();
	static bool DisableClipboardStartupWarning();
	static bool QueryKeyForValue(HKEY, wchar_t*);
};
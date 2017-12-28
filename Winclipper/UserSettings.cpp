#include "stdafx.h"
#include <string>
#include <limits>
#include <vector>
#include <map>
#include <thread>
#include "Shlobj.h"
#include "Shlwapi.h"
#include "File.h"
#include "UserSettings.h"

// stous - string to unsigned short
// NOTE: May move to static utility class in future
unsigned short stous(std::string str)
{
    try
    {
        int convint = stoi(str);

        if (convint < (std::numeric_limits<unsigned short>::min)())
        {
            throw std::range_error("Converted sequence below minimum range of unsigned short");
        }
        if (convint > (std::numeric_limits<unsigned short>::max)())
        {
            throw std::range_error("Converted sequence above maximum range of unsigned short");
        }

        return (unsigned short)convint;
    }
    catch(std::exception &e)
    {
		printf("An exception occured at %p", &e);
        throw;
    }
}

// stous - wstring to unsigned short
// NOTE: May move to static utility class in future
unsigned short stous(std::wstring str)
{
    try
    {
        int convint = stoi(str);

        if (convint < (std::numeric_limits<unsigned short>::min)())
        {
            throw std::range_error("Converted sequence below minimum range of unsigned short");
        }
        if (convint > (std::numeric_limits<unsigned short>::max)())
        {
            throw std::range_error("Converted sequence above maximum range of unsigned short");
        }

        return (unsigned short)convint;
    }
    catch (std::exception &e)
    {
		printf("An exception occured at %p", &e);
        throw;
    }
}

// Attempt to load any settings from the disk if they exist.
// If no settings can be found, set all defaults for first time run. 
// They will only be written to disk once a change has been made.
UserSettings::UserSettings()
{
    wchar_t* tempSettingPath;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempSettingPath)))
    {
        wcscpy_s(fullSettingPath, tempSettingPath);
        PathAppend(fullSettingPath, L"\\Winclipper\\Winclipper\\settings.dat");
        CoTaskMemFree(tempSettingPath);
    }

    if (File::Exists(fullSettingPath))
    {
        std::vector<std::wstring> settings = File::ReadAllLines(fullSettingPath);

        if (!settings.empty())
        {
            this->Deserialize(settings);
        }
    }
    else
    {
        // Set all defaults for first time run
        SetMaxDisplayClips(20);
        SetMaxSavedClips(50);
        SetMenuDisplayChars(64);
        SetClipsMenuHotkey(CMENU_HOTKEY_DEF);
        SetSaveToDisk(SAVE_TO_DISK_DEF);
        SetSelect2ndClip(SLCT_2ND_CLIP_DEF);
    }
}

// Not used since this object should exist for the life of the program.
UserSettings::~UserSettings()
{
}

BOOL UserSettings::NoPendingSettingWrites()
{
	return settingWriterWaitCount == 0 && isWriterFinished;
}

int UserSettings::MaxDisplayClips()
{
    if (maxDisplayClips < MAX_DISPLAY_LOWER)
    {
        maxDisplayClips = MAX_DISPLAY_LOWER;
    }
    else if (maxDisplayClips > MAX_DISPLAY_UPPER)
    {
        maxDisplayClips = MAX_DISPLAY_UPPER;
    }
    return maxDisplayClips;
}

void UserSettings::SetMaxDisplayClips(int maxDisplayClips)
{
    if (maxDisplayClips != UserSettings::maxDisplayClips)
    {
        if (maxDisplayClips < MAX_DISPLAY_LOWER)
        {
            UserSettings::maxDisplayClips = MAX_DISPLAY_LOWER;
        }
        else if (maxDisplayClips > MAX_DISPLAY_UPPER)
        {
            UserSettings::maxDisplayClips = MAX_DISPLAY_UPPER;
        }
        else
        {
            UserSettings::maxDisplayClips = maxDisplayClips;
        }
        SaveSettingsAsync();
    }
}

int UserSettings::MaxSavedClips()
{

    if (maxSavedClips < MAX_SAVED_LOWER)
    {
        maxSavedClips = MAX_SAVED_LOWER;
    }
    else if (maxSavedClips > MAX_SAVED_UPPER)
    {
        maxSavedClips = MAX_SAVED_UPPER;
    }
    return maxSavedClips;
}

void UserSettings::SetMaxSavedClips(int maxSavedClips)
{
    if (maxSavedClips != UserSettings::maxSavedClips)
    {
        if (maxSavedClips < MAX_SAVED_LOWER)
        {
            UserSettings::maxSavedClips = MAX_SAVED_LOWER;
        }
        else if (maxSavedClips > MAX_SAVED_UPPER)
        {
            UserSettings::maxSavedClips = MAX_SAVED_UPPER;
        }
        else
        {
            UserSettings::maxSavedClips = maxSavedClips;
        }
        SaveSettingsAsync();
    }
}

int UserSettings::MenuDisplayChars()
{
    if (menuDisplayChars < MENU_CHARS_LOWER)
    {
        menuDisplayChars = MENU_CHARS_LOWER;
    }
    else if (menuDisplayChars > MENU_CHARS_UPPER)
    {
        menuDisplayChars = MENU_CHARS_UPPER;
    }
    return menuDisplayChars;
}

void UserSettings::SetMenuDisplayChars(int menuDisplayChars)
{
    if (menuDisplayChars != UserSettings::menuDisplayChars)
    {
        if (menuDisplayChars < MENU_CHARS_LOWER)
        {
            UserSettings::menuDisplayChars = MENU_CHARS_LOWER;
        }
        else if (menuDisplayChars > MENU_CHARS_UPPER)
        {
            UserSettings::menuDisplayChars = MENU_CHARS_UPPER;
        }
        else
        {
            UserSettings::menuDisplayChars = menuDisplayChars;
        }
        SaveSettingsAsync();
    }
}

WORD UserSettings::ClipsMenuHotkey()
{
    if (clipsMenuHotkey == 0)
    {
        clipsMenuHotkey = CMENU_HOTKEY_DEF;
    }
    return clipsMenuHotkey;
}

// This translates the ClipsMenuHotkey values (which are
// what Winclipper stores internally) into the modifier
// keys used by the RegisterHotkey method.
//
// For some reason MS thought it was a good idea to
// flip the values used by SHIFT and ALT for the
// two different kinds of hotkeys.
WORD UserSettings::ClipsMenuHotkeyTrl()
{
    if (clipsMenuHotkey == 0)
    {
        clipsMenuHotkey = CMENU_HOTKEY_DEF;
    }

    BYTE lByte = LOBYTE(clipsMenuHotkey);
    BYTE hByte = HIBYTE(clipsMenuHotkey);

    BYTE hByteAdjusted = 0;

    if ((hByte & HOTKEYF_SHIFT) == HOTKEYF_SHIFT)
    {
        hByteAdjusted |= MOD_SHIFT;
    }
    if ((hByte & HOTKEYF_CONTROL) == HOTKEYF_CONTROL)
    {
        hByteAdjusted |= MOD_CONTROL;
    }
    if ((hByte & HOTKEYF_ALT) == HOTKEYF_ALT)
    {
        hByteAdjusted |= MOD_ALT;
    }
    if ((hByte & HOTKEYF_EXT) == HOTKEYF_EXT)
    {
        hByteAdjusted |= MOD_WIN;
    }

    WORD retVal = MAKEWORD(lByte, hByteAdjusted);

    return retVal;
}

void UserSettings::SetClipsMenuHotkey(WORD clipsMenuHotkey)
{
    if (clipsMenuHotkey != UserSettings::clipsMenuHotkey)
    {
        UserSettings::clipsMenuHotkey = clipsMenuHotkey;
        SaveSettingsAsync();
    }
}

bool UserSettings::SaveToDisk()
{
    return UserSettings::saveToDisk;
}

void UserSettings::SetSaveToDisk(bool saveToDisk)
{
    if (saveToDisk != UserSettings::saveToDisk)
    {
        UserSettings::saveToDisk = saveToDisk;
        SaveSettingsAsync();
    }
}

bool UserSettings::Select2ndClip()
{
    return UserSettings::select2ndClip;
}

void UserSettings::SetSelect2ndClip(bool select2ndClip)
{
    if (select2ndClip != UserSettings::select2ndClip)
    {
        UserSettings::select2ndClip = select2ndClip;
        SaveSettingsAsync();
    }
}

// Writes a serialized version of the current settings to disk.
void UserSettings::WriteSettings()
{
	isWriterFinished = FALSE;
    std::vector<std::wstring> settings = this->Serialize();

    File::WriteAllLines(fullSettingPath, settings);
	isWriterFinished = TRUE;
}

// Returns a vector of type std::wstring containing std::wstring
// representations of all current settings.
std::vector<std::wstring> UserSettings::Serialize()
{
    std::vector<std::wstring> retVal = {
        (std::to_wstring(MAX_DISPLAY) + SEPARATOR + std::to_wstring(MaxDisplayClips())),
        (std::to_wstring(MAX_SAVED) + SEPARATOR + std::to_wstring(MaxSavedClips())),
        (std::to_wstring(MENU_CHARS) + SEPARATOR + std::to_wstring(MenuDisplayChars())),
        (std::to_wstring(CMENU_HOTKEY) + SEPARATOR + std::to_wstring(ClipsMenuHotkey())),
        (std::to_wstring(SAVE_TO_DISK) + SEPARATOR + std::to_wstring(SaveToDisk())),
        (std::to_wstring(SLCT_2ND_CLIP) + SEPARATOR + std::to_wstring(Select2ndClip()))
    };

    return retVal;
}

// Deserializes a vector of type std::wstring into setting values,
// and then sets them where availble. 
// Define a constant for each setting, and then add it to the switch. 
// Expects a format of:
// 1|settingValue1
// 2|settingValue2
// n|settingValueN
void UserSettings::Deserialize(std::vector<std::wstring> srData)
{
    std::map<int, std::wstring> store;

    for each (std::wstring kvPair in srData)
    {
        try
        {
            store.emplace(
                stoi(kvPair.substr(0, kvPair.find(SEPARATOR))),
                kvPair.substr(kvPair.find(SEPARATOR) + 1)
            );
        }
        catch (const std::exception&)
        {
            // do nothing
        }
    }

    for each (std::pair<int, std::wstring> pair in store)
    {
        switch (pair.first)
        {
        case MAX_DISPLAY:
            SetMaxDisplayClips(stoi(pair.second));
            break;
        case MAX_SAVED:
            SetMaxSavedClips(stoi(pair.second));
            break;
        case MENU_CHARS:
            SetMenuDisplayChars(stoi(pair.second));
            break;
        case CMENU_HOTKEY:
            SetClipsMenuHotkey(stous(pair.second));
            break;
        case SAVE_TO_DISK:
            {
                bool result = pair.second.compare(std::to_wstring(true)) == 0;
                SetSaveToDisk(result);
            }
            break;
        case SLCT_2ND_CLIP:
            {
                bool result = pair.second.compare(std::to_wstring(true)) == 0;
                SetSelect2ndClip(result);
            }
        break;
        default:
            break;
        }
    }
}

// Schedules a write operation to the settings file.
void UserSettings::SaveSettingsAsync()
{
	if (settingWriterWaitCount == 0)
	{
		settingWriterWaitCount++;
		std::thread([&]() {DelaySettingWriter(&settingWriterWaitCount, this); }).detach();
	}
	else
	{
		settingWriterWaitCount++;
	}
}

// Sets the value pointed to by waitCount to 1, sleeps for an
// amount of time, decrements the value by 1, and repeats if it does
// not have a value of zero. If it does, it writes the settings file to disk.
void UserSettings::DelaySettingWriter(int* waitCount, UserSettings* us)
{
	while (*waitCount != 0)
	{
		// we wait to make sure that no aditional actions
		// have been taken since we started waiting.
		(*waitCount) = 1;
		Sleep(WRITE_DELAY);
		(*waitCount)--;
	}

	us->WriteSettings();
}
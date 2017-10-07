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
        throw;
    }
}

// Attempt to load any settings from the disk if they exist.
// If no settings can be found, set all defaults for first time run. 
// They will only be written to disk once a change has been made.
UserSettings::UserSettings()
{
    TCHAR* tempSettingPath;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempSettingPath)))
    {
        _tcscpy_s(fullSettingPath, tempSettingPath);
        PathAppend(fullSettingPath, _T("\\Winclipper\\Winclipper\\settings.dat"));
        CoTaskMemFree(tempSettingPath);
    }

    if (File::Exists(fullSettingPath))
    {
        std::vector<TSTRING> settings = File::ReadAllLines(fullSettingPath);

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
    std::vector<TSTRING> settings = this->Serialize();

    File::WriteAllLines(fullSettingPath, settings);
}

// Returns a vector of type TSTRING containing TSTRING
// representations of all current settings.
std::vector<TSTRING> UserSettings::Serialize()
{
    std::vector<TSTRING> retVal = {
        (TO_TSTRING(MAX_DISPLAY) + SEPARATOR + TO_TSTRING(MaxDisplayClips())),
        (TO_TSTRING(MAX_SAVED) + SEPARATOR + TO_TSTRING(MaxSavedClips())),
        (TO_TSTRING(MENU_CHARS) + SEPARATOR + TO_TSTRING(MenuDisplayChars())),
        (TO_TSTRING(CMENU_HOTKEY) + SEPARATOR + TO_TSTRING(ClipsMenuHotkey())),
        (TO_TSTRING(SAVE_TO_DISK) + SEPARATOR + TO_TSTRING(SaveToDisk())),
        (TO_TSTRING(SLCT_2ND_CLIP) + SEPARATOR + TO_TSTRING(Select2ndClip()))
    };

    return retVal;
}

// Deserializes a vector of type TSTRING into setting values,
// and then sets them where availble. 
// Define a constant for each setting, and then add it to the switch. 
// Expects a format of:
// 1|settingValue1
// 2|settingValue2
// n|settingValueN
void UserSettings::Deserialize(std::vector<TSTRING> srData)
{
    std::map<int, TSTRING> store;

    for each (TSTRING kvPair in srData)
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

    for each (std::pair<int, TSTRING> pair in store)
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
                bool result = pair.second.compare(TO_TSTRING(true)) == 0;
                SetSaveToDisk(result);
            }
            break;
        case SLCT_2ND_CLIP:
            {
                bool result = pair.second.compare(TO_TSTRING(true)) == 0;
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
    std::thread([&]() {IncrementSettingWriterDelay(&settingWriterWaitCount, this); }).detach();
}

// Increments the value pointed to by waitCount by 1, sleeps for an
// amount of time, decrements the value by 1, and then tests to see if it
// has a value of zero. If so, it writes the settings file to disk.
void UserSettings::IncrementSettingWriterDelay(int* waitCount, UserSettings* us)
{
    (*waitCount)++;
    Sleep(WRITE_DELAY);
    (*waitCount)--;

    if (*waitCount == 0)
    {
        us->WriteSettings();
    }
}
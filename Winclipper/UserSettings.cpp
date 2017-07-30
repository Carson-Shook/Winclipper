#include "stdafx.h"
#include <string>
#include <vector>
#include <map>
#include <thread>
#include "Shlobj.h"
#include "File.h"
#include "UserSettings.h"

UserSettings::UserSettings()
{
    TCHAR* tempSettingPath;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempSettingPath);
    _tcsncat_s(fullSettingPath, MAX_PATH, tempSettingPath, _tcslen(tempSettingPath));
    _tcsncat_s(fullSettingPath, MAX_PATH, settingFilePath, _tcslen(settingFilePath));
    CoTaskMemFree(tempSettingPath);

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
        SetMaxDisplayClips(20);
        SetMaxSavedClips(50);
        SetMenuDisplayChars(64);
    }
}

UserSettings::~UserSettings()
{
}

int UserSettings::MaxDisplayClips()
{
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

void UserSettings::WriteSettings()
{
    std::vector<TSTRING> settings = this->Serialize();

    File::WriteAllLines(fullSettingPath, settings);
}

std::vector<TSTRING> UserSettings::Serialize()
{
    std::vector<TSTRING> retVal = {
        (TO_TSTRING(MAX_DISPLAY) + SEPARATOR + TO_TSTRING(MaxDisplayClips())),
        (TO_TSTRING(MAX_SAVED) + SEPARATOR + TO_TSTRING(MaxSavedClips())),
        (TO_TSTRING(MENU_CHARS) + SEPARATOR + TO_TSTRING(MenuDisplayChars()))
    };

    return retVal;
}

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
        default:
            break;
        }
    }
}

// Call this to schedule a write to the settings file.
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
    Sleep(2000);
    (*waitCount)--;

    if (*waitCount == 0)
    {
        us->WriteSettings();
    }
}
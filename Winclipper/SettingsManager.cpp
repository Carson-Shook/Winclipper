#include "stdafx.h"
#include <string>
#include <vector>
#include <thread>
#include "Shlobj.h"
#include "File.h"
#include "SettingsManager.h"

UserSettings::UserSettings()
{
    TCHAR* tempSettingPath;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempSettingPath);
    _tcsncat_s(fullSettingPath, MAX_PATH, tempSettingPath, _tcslen(tempSettingPath));
    _tcsncat_s(fullSettingPath, MAX_PATH, settingFilePath, _tcslen(settingFilePath));
    CoTaskMemFree(tempSettingPath);

    std::vector<TSTRING> settings = File::ReadAllLines(fullSettingPath);

    SetMaxDisplayClips(stoi(settings[0]));
    SetMaxSavedClips(stoi(settings[1]));
    //maxDisplayClips = 20;
    //maxSavedClips = 100;
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
        UserSettings::maxDisplayClips = maxDisplayClips;
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
        UserSettings::maxSavedClips = maxSavedClips;
        SaveSettingsAsync();
    }
}

void UserSettings::WriteSettings()
{
    std::vector<TSTRING> settings = {
        TO_TSTRING(MaxDisplayClips()),
        TO_TSTRING(MaxSavedClips())
    };

    File::WriteAllLines(fullSettingPath, settings);
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
    Sleep(3000);
    (*waitCount)--;

    if (*waitCount == 0)
    {
        us->WriteSettings();
    }
}
#pragma once
// Reads and writes settings to and from
// the current users's AppData folder
// and makes them available to read or change
#include <vector>
#define SEPARATOR           _T("|")

#define MAX_DISPLAY         1
#define MAX_DISPLAY_UPPER   99
#define MAX_DISPLAY_LOWER   1

#define MAX_SAVED           2
#define MAX_SAVED_UPPER     500
#define MAX_SAVED_LOWER     1

class UserSettings
{
private:
    TCHAR               fullSettingPath[MAX_PATH];
    TCHAR*              settingFilePath = _T("\\Winclipper\\Winclipper\\settings.dat");
    int                 settingWriterWaitCount = 0;

    int                 maxDisplayClips;
    int                 maxSavedClips;

    void                SaveSettingsAsync();
    static void         IncrementSettingWriterDelay(int* waitCount, UserSettings* us);
    void                WriteSettings();

protected:
    std::vector<TSTRING>  Serialize();
    void                  Deserialize(std::vector<TSTRING> srData);

public:
    UserSettings();
    ~UserSettings();

    int                 MaxDisplayClips();
    void                SetMaxDisplayClips(int maxDisplayClips);

    int                 MaxSavedClips();
    void                SetMaxSavedClips(int maxSavedClips);
};



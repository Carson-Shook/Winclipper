#pragma once
// Reads and writes settings to and from
// the current users's AppData folder
// and makes them available to read or change
#include <vector>
#define SEPARATOR           _T("|")
#define WRITE_DELAY         2000

#define MAX_DISPLAY         1
#define MAX_DISPLAY_UPPER   99
#define MAX_DISPLAY_LOWER   1

#define MAX_SAVED           2
#define MAX_SAVED_UPPER     500
#define MAX_SAVED_LOWER     1

#define MENU_CHARS          3
#define MENU_CHARS_UPPER    200
#define MENU_CHARS_LOWER    1

#define CMENU_HOTKEY        4
#define CMENU_HOTKEY_DEF    MAKEWORD(0x56, HOTKEYF_CONTROL | HOTKEYF_SHIFT)

#define SAVE_TO_DISK        5
#define SAVE_TO_DISK_DEF    true

// Save and load user settings to and from disk
class UserSettings
{
private:
    TCHAR               fullSettingPath[MAX_PATH];
    TCHAR*              settingFilePath = _T("\\Winclipper\\Winclipper\\settings.dat");
    int                 settingWriterWaitCount = 0;

    int                 maxDisplayClips;
    int                 maxSavedClips;
    int                 menuDisplayChars;
    WORD                clipsMenuHotkey;
    bool                saveToDisk = SAVE_TO_DISK_DEF;

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

    int                 MenuDisplayChars();
    void                SetMenuDisplayChars(int menuDisplayChars);

    WORD                ClipsMenuHotkey();
    WORD                ClipsMenuHotkeyTrl();
    void                SetClipsMenuHotkey(WORD clipsMenuHotkey);

    bool                SaveToDisk();
    void                SetSaveToDisk(bool saveToDisk);
};



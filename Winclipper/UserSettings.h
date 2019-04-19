#pragma once
// Reads and writes settings to and from
// the current users's AppData folder
// and makes them available to read or change
#include "Notify.h"
#include "resource.h"
#include "Shlobj.h"
#include "Shlwapi.h"
#include "File.h"
#define SEPARATOR           L"|"
#define WRITE_DELAY         2000

#define MAX_DISPLAY         1
#define MAX_DISPLAY_UPPER   99
#define MAX_DISPLAY_LOWER   1

#define MAX_SAVED           2
#define MAX_SAVED_UPPER     500
#define MAX_SAVED_LOWER     1

#define MENU_CHARS          3
#define MENU_CHARS_UPPER    200
#define MENU_CHARS_LOWER    5

#define CMENU_HOTKEY        4
#define CMENU_HOTKEY_DEF    MAKEWORD(0x56, HOTKEYF_CONTROL | HOTKEYF_SHIFT)

#define SAVE_TO_DISK        5
#define SAVE_TO_DISK_DEF    true

#define SLCT_2ND_CLIP       6
#define SLCT_2ND_CLIP_DEF   true

#define SHOW_PREVIEW        7
#define SHOW_PREVIEW_DEF    true

// Save and load user settings to and from disk
class UserSettings : public Notify
{
private:
    std::wstring        fullSettingPath;
    int                 maxDisplayClips;
    int                 maxSavedClips;
    int                 menuDisplayChars;
    WORD                clipsMenuHotkey;
    bool                saveToDisk = SAVE_TO_DISK_DEF;
    bool                select2ndClip;
    bool                showPreview;

    int                 settingWriterWaitCount = 0;
    bool                isWriterFinished = true;

    void                SaveSettingsAsync();
    static void         DelaySettingWriter(int* waitCount, UserSettings* us);
    void                WriteSettings();

protected:
    std::vector<std::wstring>  Serialize();
    void                  Deserialize(std::vector<std::wstring> srData);

public:
    UserSettings();
    ~UserSettings();

    bool                NoPendingSettingWrites();

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

    bool                Select2ndClip();
    void                SetSelect2ndClip(bool select2ndClip);

    bool                ShowPreview();
    void                SetShowPreview(bool showPreview);

};



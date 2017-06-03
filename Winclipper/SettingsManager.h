#pragma once
// Reads and writes settings to and from
// the current users's AppData folder
// and makes them available to read or change

#define MAX_DISPLAY_UPPER   99
#define MAX_DISPLAY_LOWER   1

#define MAX_SAVED_UPPER    500
#define MAX_SAVED_LOWER    1

class UserSettings
{
public:
    UserSettings();
    ~UserSettings();

    int                 MaxDisplayClips();
    void                SetMaxDisplayClips(int maxDisplayClips);

    int                 MaxSavedClips();
    void                SetMaxSavedClips(int maxSavedClips);

private:
    int                 maxDisplayClips;
    int                 maxSavedClips;
};



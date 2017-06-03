#pragma once
// Reads and writes settings to and from
// the current users's AppData folder
// and makes them available to read or change

#define MAX_DISPLAY_UPPER   99
#define MAX_DISPLAY_LOWER   1

#define MAX_STORED_UPPER    500
#define MAX_STORED_LOWER    1

class UserSettings
{
public:
    UserSettings();
    ~UserSettings();

    int                 MaxDisplayClips();
    void                SetMaxDisplayClips(int);

    int                 MaxStoredClips();
    void                SetMaxStoredClips(int);

private:
    int                 maxDisplayClips;
    int                 maxStoredClips;
};



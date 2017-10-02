#pragma once

#define CANCELED_SELECTION	0		// user selected nothing, pressed esc, or clicked outside the window
#define	SETTINGS_SELECT     2000	// user selected the Settings option
#define CLEARCLIPS_SELECT   2001    // clear all clips that are currently saved (doesn't clear the actual clipboard)
#define EXIT_SELECT         2002    // close the application

									// ERROR is returned in any other situation

class ClipsManager
{
private:
    TCHAR                           fullClipsPath[MAX_PATH];
    std::deque<TCHAR *>             clips;
    size_t                          displayClips = 20;
    size_t                          maxClips = 200;         // Please keep it below 999
    int                             menuDispChars = 64;
    bool                            saveToDisk = false;

    int                             clipsWriterWaitCount = 0;

    void                            SaveClipsAsync();
    static void                     IncrementClipsWriterDelay(int* waitCount, ClipsManager* cs);
    void                            WriteClips();
    void                            ReadClips();

public:
    ClipsManager();
    ClipsManager(int displayClips, int maxClips, int menuChars);
    ~ClipsManager();

    int                             DisplayClips();
    void                            SetDisplayClips(int displayClips);

    int                             MaxClips();
    void                            SetMaxClips(int maxClips);

    int                             MenuDisplayChars();
    void                            SetMenuDisplayChars(int menuDispChars);

    bool                            SaveToDisk();
    void                            SetSaveToDisk(bool saveToDisk);

    void                            ClearClips(void);
    BOOL                            AddToClips(HWND hWnd);
    BOOL                            SetClipboardToClipAtIndex(HWND hWnd, int index);
    const std::deque<TCHAR *>&      GetClips(void) const { return clips; }
};

// Forward declarations of functions included in this code module:
void				ShowClipsMenu(HWND hWnd, HWND curWin, ClipsManager& cm, bool showExit);
void				SendPasteInput(void);
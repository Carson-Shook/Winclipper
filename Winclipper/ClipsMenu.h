#pragma once

#define CANCELED_SELECTION	0		// user selected nothing, pressed esc, or clicked outside the window
#define	SETTINGS_SELECT     2000	// user selected the Settings option
#define CLEARCLIPS_SELECT   2001    // clear all clips that are currently saved (doesn't clear the actual clipboard)
#define EXIT_SELECT         2002    // close the application
#define CLIPS_WRITE_DELAY   2000	

									// ERROR is returned in any other situation

class ClipsManager
{
private:
    wchar_t                         fullClipsPath[MAX_PATH];
    std::deque<wchar_t *>           clips;
    unsigned int                    displayClips = 20;
    unsigned int                    maxClips = 200;         // Please keep it below 999
    unsigned int                    menuDispChars = 64;
    bool                            saveToDisk = true;

    int                             clipsWriterWaitCount = 0;
	BOOL							isWriterFinished = TRUE;

    void                            SaveClipsAsync();
    static void                     DelayClipsWriter(int* waitCount, ClipsManager* cs);
    void                            WriteClips();
    void                            ReadClips();

public:
    ClipsManager();
    ClipsManager(int displayClips, int maxClips, int menuChars, bool saveToDisk);
    ~ClipsManager();

	BOOL							NoPendingClipWrites();

    unsigned int                    DisplayClips();
    void                            SetDisplayClips(unsigned int displayClips);

    unsigned int                    MaxClips();
    void                            SetMaxClips(unsigned int maxClips);

    unsigned int                    MenuDisplayChars();
    void                            SetMenuDisplayChars(unsigned int menuDispChars);

    bool                            SaveToDisk();
    void                            SetSaveToDisk(bool saveToDisk);

    void                            ClearClips(void);
    BOOL                            AddToClips(HWND hWnd);
    BOOL                            SetClipboardToClipAtIndex(HWND hWnd, int index);
    const std::deque<wchar_t *>&    GetClips(void) const { return clips; }
};

// Forward declarations of functions included in this code module:
void				ShowClipsMenu(HWND hWnd, ClipsManager& cm, bool showExit);
void				SelectDefaultMenuItem(bool select2ndClip);
void				SendPasteInput(void);
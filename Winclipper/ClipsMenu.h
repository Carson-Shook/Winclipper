#pragma once

#define MENU_TEXT_LENGTH    64
#define CANCELED_SELECTION	0		// user selected nothing, pressed esc, or clicked outside the window
#define	SETTINGS_SELECT     1000	// user selected the Settings option
#define CLEARCLIPS_SELECT   1001    // clear all clips that are currently saved (doesn't clear the actual clipboard)
#define EXIT_SELECT         1002    // close the application

									// ERROR is returned in any other situation

class ClipsManager
{
public:
    ClipsManager();
    ~ClipsManager();

    int                             DisplayClips(void);
    void                            SetDisplayClips(int displayClips);

    int                             MaxClips(void);
    void                            SetMaxClips(int maxClips);

    void                            ClearClips(void);
    BOOL                            AddToClips(HWND hWnd);
    BOOL                            SetClipboardToClipAtIndex(HWND hWnd, int index);
    const std::deque<TCHAR *>&      GetClips(void) const { return clips; }

private:
    std::deque<TCHAR *> clips;
    int displayClips = 20;
    int maxClips = 200;              // Should never be greater than 999
};

// Forward declarations of functions included in this code module:
int					ShowClipsMenu(HWND hWnd, HWND curWin, ClipsManager& cm, bool showExit);
void				SendPasteInput(void);
#pragma once

#define MENU_TEXT_LENGTH    84
#define CANCELED_SELECTION	0		// user selected nothing, pressed esc, or clicked outside the window
#define	SETTINGS_SELECT     1000	// user selected the Settings option
#define CLEARCLIPS_SELECT   1001    // clear all clips that are currently saved (doesn't clear the actual clipboard)
									// ERROR is returned in any other situation

class ClipsManager
{
public:
    ClipsManager();
    ~ClipsManager();

    void                            ClearClips();
    BOOL                            AddToClips(HWND hWnd);
    BOOL                            SetClipboardToClipAtIndex(HWND, int);
    const std::deque<wchar_t *>&    GetClips() const { return clips; }

    void                            SetMaxClips(int newMax) { maxClips = newMax; }
    int                             GetMaxClips() { return maxClips; }

private:
    std::deque<wchar_t *> clips;
    int maxClips = 10;              // Should never be greater than 999
};

// Forward declarations of functions included in this code module:
int					ShowClipsMenu(HWND, HWND, ClipsManager&);
void				SendPasteInput(void);
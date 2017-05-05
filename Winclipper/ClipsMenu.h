#pragma once
#include <deque>

#define MENU_TEXT_LENGTH    84
#define CANCELED_SELECTION	1		// user selected nothing, pressed esc, or clicked outside the window
#define SELECTED_PASTE_ITEM	2		// user selected one of the clips, and it was pasted 
#define	SELECTED_SETTINGS	99		// user selected the Settings option
									// ERROR is returned in any other situation

class ClipsManager
{
public:
    ClipsManager();
    ~ClipsManager();

    BOOL                            AddToClips(HWND hWnd);
    BOOL                            SetClipboardToClipAtIndex(HWND, int);
    const std::deque<wchar_t *>&    GetClips() const { return clips; }

    void                            SetMaxClips(int newMax) { maxClips = newMax; }
    int                             GetMaxClips() { return maxClips; }

private:
    std::deque<wchar_t *> clips;
    int maxClips = 10;
};

// Forward declarations of functions included in this code module:
int					ShowClipsMenu(HWND, HWND, ClipsManager&);
void				SendPasteInput(void);
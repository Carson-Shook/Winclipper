#pragma once

#define CANCELED_SELECTION	1		// user selected nothing, pressed esc, or clicked outside the window
#define SELECTED_PASTE_ITEM	2		// user selected one of the clips, and it was pasted 
#define	SELECTED_SETTINGS	99		// user selected the Settings option
									// ERROR is returned in any other situation

// Forward declarations of functions included in this code module:
BOOL				AddToClips(HWND);
BOOL				MoveToFrontOfClips(HWND);
int					ShowClipsMenu(HWND, HWND);
void				SendPasteInput(void);
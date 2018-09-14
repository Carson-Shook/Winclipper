#pragma once
#include "resource.h"
#include "UserSettings.h"
#include "File.h"
#include "Shlobj.h"
#include "Shlwapi.h"

#define CANCELED_SELECTION     0    // user selected nothing, pressed esc, or clicked outside the window
#define SETTINGS_SELECT     2000    // user selected the Settings option
#define CLEARCLIPS_SELECT   2001    // clear all clips that are currently saved (doesn't clear the actual clipboard)
#define EXIT_SELECT         2002    // close the application
#define CLIPS_WRITE_DELAY   2000    

                                    // ERROR is returned in any other situation

typedef struct {
	DWORD pId = 0;
	HWND hWnd = nullptr;
} HANDLEDATA;

class ClipsManager
{
private:
    wchar_t                         fullClipsPath[MAX_PATH];
    std::deque<wchar_t *>           clips;
    unsigned int                    displayClips = 20;
    unsigned int                    maxClips = 200;
    unsigned int                    menuDispChars = 64;
    bool                            saveToDisk = true;

    int                             clipsWriterWaitCount = 0;
    bool                            isWriterFinished = true;

    void                            SaveClipsAsync();
    static void                     DelayClipsWriter(int* waitCount, ClipsManager* cs);
    void                            WriteClips();
    void                            ReadClips();
	void							SendPasteInput(void);

public:
    ClipsManager();
    ClipsManager(int displayClips, int maxClips, int menuChars, bool saveToDisk);
    ~ClipsManager();

    bool                            NoPendingClipWrites();

    unsigned int                    DisplayClips();
    void                            SetDisplayClips(unsigned int displayClips);

    unsigned int                    MaxClips();
    void                            SetMaxClips(unsigned int maxClips);

    unsigned int                    MenuDisplayChars();
    void                            SetMenuDisplayChars(unsigned int menuDispChars);

    bool                            SaveToDisk();
    void                            SetSaveToDisk(bool saveToDisk);

    void                            ClearClips(void);
    bool                            AddToClips(HWND hWnd);
    bool                            SetClipboardToClipAtIndex(HWND hWnd, int index);
	const size_t					GetSize() noexcept;
	wchar_t *						GetClipAt(size_t index) noexcept;
	void							ShowClipsMenu(HWND hWnd, LPPOINT cPos, bool showExit);
	void							SelectDefaultMenuItem(bool select2ndClip) noexcept;
};
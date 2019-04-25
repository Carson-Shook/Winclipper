#pragma once
#include "resource.h"
#include "UserSettings.h"
#include "File.h"
#include "RegistryUtilities.h"
#include "Clip.h"
#include "ClipsCollection.h"
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
	ClipsManager();
	unsigned int					windows10ReleaseId = 0;

    std::wstring                    fullClipsPath;
    ClipsCollection					clips;
    unsigned int                    displayClips = 20;
    unsigned int                    maxClips = 200;
    unsigned int                    menuDispChars = 64;
	bool                            saveToDisk = true;
	bool                            saveImages = true;

    int                             clipsWriterWaitCount = 0;
    bool                            isWriterFinished = true;

    void                            SaveClipsAsync();
    static void                     DelayClipsWriter(int* waitCount, ClipsManager* cs);
    void                            WriteClips();
    void                            ReadClips();
	unsigned int					SendPasteInput(void) noexcept;
	void							ClearBitmaps();

public:
    ClipsManager(int displayClips, int maxClips, int menuChars, bool saveToDisk, bool saveImages);
    ~ClipsManager();

    bool                            NoPendingClipWrites() noexcept;
	bool							ClipLock = false;

    unsigned int                    DisplayClips() noexcept;
    void                            SetDisplayClips(unsigned int displayClips) noexcept;

    unsigned int                    MaxClips() noexcept;
    void                            SetMaxClips(unsigned int maxClips);

    unsigned int                    MenuDisplayChars() noexcept;
    void                            SetMenuDisplayChars(unsigned int menuDispChars) noexcept;

    bool                            SaveToDisk() noexcept;
    void                            SetSaveToDisk(bool saveToDisk);

	bool                            SaveImages() noexcept;
	void                            SetSaveImages(bool saveToDisk);

	void                            RecreateThumbnails();

    void                            ClearClips(void);
    bool                            AddToClips(HWND hWnd);
    bool                            SetClipboardToClipAtIndex(HWND hWnd, unsigned int index);
	const unsigned int				GetSize() noexcept;
	void							DeleteClipAt(unsigned int index);
	std::shared_ptr<Clip>			GetClipAt(unsigned int index);
	void							ShowClipsMenu(HWND hWnd, const LPPOINT cPos, bool showExit);
	void							SelectDefaultMenuItem(bool select2ndClip) noexcept;
};
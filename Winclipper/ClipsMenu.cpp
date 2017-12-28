#include "stdafx.h"
#include "resource.h"
#include <thread>
#include "UserSettings.h"
#include "File.h"
#include "Shlobj.h"
#include "Shlwapi.h"
#include "ClipsMenu.h"

void ClipsManager::SaveClipsAsync()
{
	if (SaveToDisk())
	{
		if (clipsWriterWaitCount == 0)
		{
			clipsWriterWaitCount++;
			std::thread([&]() {DelayClipsWriter(&clipsWriterWaitCount, this); }).detach();
		}
		else
		{
			clipsWriterWaitCount++;
		}
    }
    else
    {
        if (File::Exists(fullClipsPath))
        {
            File::Delete(fullClipsPath);
        }
    }
}

void ClipsManager::DelayClipsWriter(int * waitCount, ClipsManager * cs)
{
	while (*waitCount != 0)
	{
		// we wait to make sure that no aditional actions
		// have been taken since we started waiting.
		(*waitCount) = 1;
		Sleep(CLIPS_WRITE_DELAY);
		(*waitCount)--;
	}
        
	cs->WriteClips();
}

void ClipsManager::WriteClips()
{
	isWriterFinished = FALSE;
    File::BinaryWriteDeque(GetClips(), fullClipsPath);
	isWriterFinished = TRUE;
}

void ClipsManager::ReadClips()
{
    if (File::Exists(fullClipsPath))
    {
        std::deque<wchar_t *> tempClips = File::BinaryReadDeque(fullClipsPath);

        clips.assign(tempClips.begin(), tempClips.end());

        tempClips.clear();
    }
}

ClipsManager::ClipsManager()
{
}

ClipsManager::ClipsManager(int displayClips, int maxClips, int menuChars, bool saveToDisk)
{
    PTSTR tempClipsPath;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempClipsPath)))
    {
        wcscpy_s(fullClipsPath, tempClipsPath);
        PathAppend(fullClipsPath, L"\\Winclipper\\Winclipper\\clips.dat");
        CoTaskMemFree(tempClipsPath);
    }

	ClipsManager::saveToDisk = saveToDisk;
    ReadClips();
    SetDisplayClips(displayClips);
    SetMaxClips(maxClips);
    SetMenuDisplayChars(menuChars);
}

ClipsManager::~ClipsManager()
{
    ClearClips();
}

BOOL ClipsManager::NoPendingClipWrites()
{
	return clipsWriterWaitCount == 0 && isWriterFinished;
}


unsigned int ClipsManager::DisplayClips()
{
    return displayClips;
}

void ClipsManager::SetDisplayClips(unsigned int displayClips)
{
    if (displayClips != ClipsManager::displayClips)
    {
        ClipsManager::displayClips = displayClips;
    }
}

unsigned int ClipsManager::MaxClips()
{
    return maxClips;
}

void ClipsManager::SetMaxClips(unsigned int maxClips)
{
    if (maxClips != ClipsManager::maxClips)
    {
        ClipsManager::maxClips = maxClips;

        while (clips.size() > ClipsManager::maxClips)
        {
            // get reference to the back object before we pop it
            wchar_t * back = clips.back();
            clips.pop_back();
            // dealocate the former back object
            delete[] back;
        }
        SaveClipsAsync();
    }
}

unsigned int ClipsManager::MenuDisplayChars()
{
    return menuDispChars;
}

void ClipsManager::SetMenuDisplayChars(unsigned int menuDispChars)
{
    ClipsManager::menuDispChars = menuDispChars;
}

bool ClipsManager::SaveToDisk()
{
    return ClipsManager::saveToDisk;
}

void ClipsManager::SetSaveToDisk(bool saveToDisk)
{
    if (saveToDisk != ClipsManager::saveToDisk)
    {
        ClipsManager::saveToDisk = saveToDisk;
        SaveClipsAsync();
    }
}

void ClipsManager::ClearClips()
{
    for (size_t i = 0, j = clips.size(); i < j; i++)
    {
        wchar_t * clip = clips.at(i);
        delete[] clip;
    }
    clips.clear();
    SaveClipsAsync();
}

BOOL ClipsManager::AddToClips(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
        return FALSE;
    HANDLE psClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (psClipboardData != NULL)
    {
        wchar_t * data = (wchar_t*)GlobalLock(psClipboardData);
        if (data != NULL)
        {
            wchar_t * derefData = _wcsdup(data);
            GlobalUnlock(psClipboardData);

            while (clips.size() >= maxClips)
            {
                // get reference to the back object before we pop it
                wchar_t * back = clips.back();
                clips.pop_back();
                // dealocate the former back object
                delete[] back;
            }
            clips.push_front(derefData);
        }
        else 
        {
            GlobalUnlock(psClipboardData);
        }
    }
    CloseClipboard();
    SaveClipsAsync();
    return TRUE;
}

BOOL ClipsManager::SetClipboardToClipAtIndex(HWND hWnd, int index)
{
    if (!OpenClipboard(hWnd))
        return FALSE;

    EmptyClipboard();

    wchar_t * clip = clips.at(index);

    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (wcslen(clip) + 1) * sizeof(wchar_t));
    if (hClipboardData != NULL)
    {
        wchar_t * pwcData = (wchar_t*)GlobalLock(hClipboardData);
        if (pwcData != NULL)
        {
            wcscpy_s(pwcData, wcslen(clip) + 1, clip);

            GlobalUnlock(hClipboardData);

            SetClipboardData(CF_UNICODETEXT, hClipboardData);

            // Safe to delete clip because we have already copied it.
            delete[] clip;
            clips.erase(clips.begin() + index);
        }
        else {
            GlobalUnlock(hClipboardData);
        }
    }
    // Once the clipboard is closed, the update is fired,
    // and the clip will be added to the front of the deque with AddToClips
    CloseClipboard();
    SaveClipsAsync();
    return TRUE;
}

void ShowClipsMenu(HWND hWnd, ClipsManager& cm, bool showExit)
{
	HWND curWin = GetForegroundWindow();
	if (hWnd != NULL && curWin != NULL)
	{
		// Since WindowsNT (I think), it's necessary to attach the process to the thread
		// that we want to get the active window of, otherwise we get an unspecified value.
		// GetForegroundWindow was unreliable with UWP applications because it took too long
		// to restore the active window (the text field) after the foreground window activated.
		AttachThreadInput(GetWindowThreadProcessId(curWin, NULL), GetWindowThreadProcessId(hWnd, NULL), TRUE);
		HWND actWin = GetActiveWindow();

		SetForegroundWindow(hWnd);

		LPPOINT cPos = new POINT;
		if (!GetCursorPos(cPos))
		{
            (*cPos).x = 10;
            (*cPos).y = 10;
		}

		HMENU menu = CreatePopupMenu();

        size_t curSize = cm.GetClips().size();
        unsigned int displayCount = cm.DisplayClips();
        unsigned int menuTextLength = cm.MenuDisplayChars();
        wchar_t* menuText = new wchar_t[menuTextLength];

        if (curSize > 0)
        {
            size_t j = curSize > displayCount ? displayCount : curSize;

            for (unsigned int i = 0; i < j; i++)
            {
                wchar_t * clip = cm.GetClips().at(i);
                if (clip != NULL)
                {
                    size_t maxClipSize = wcslen(clip);

                    // I know this is a little unorthodox, but we can count
                    // on these lengths so long as MENU_TEXT_LENGTH > 5
                    // and I really want these ellipses.
                    if (maxClipSize > menuTextLength - 4)
                    {
                        maxClipSize = menuTextLength - 4;
                        menuText[menuTextLength - 4] = '.';
                        menuText[menuTextLength - 3] = '.';
                        menuText[menuTextLength - 2] = '.';
                        menuText[menuTextLength - 1] = '\0';
                    }
                    else
                    {
                        menuText[maxClipSize] = '\0';
                    }

                    for (unsigned int k = 0; k < maxClipSize; k++)
                    {
                        menuText[k] = clip[k];
                    }

                    AppendMenu(menu, MF_STRING, i + 1, menuText);
                }
            }
            if (curSize > displayCount)
            {
                HMENU sMenu = CreatePopupMenu();
                
                for (/*j is equal to displayCount */; j < curSize; j++)
                {
                    wchar_t * clip = cm.GetClips().at(j);
                    if (clip != NULL)
                    {
                        size_t maxClipSize = wcslen(clip);

                        if (maxClipSize > menuTextLength - 4)
                        {
                            maxClipSize = menuTextLength - 4;
                            menuText[menuTextLength - 4] = '.';
                            menuText[menuTextLength - 3] = '.';
                            menuText[menuTextLength - 2] = '.';
                            menuText[menuTextLength - 1] = '\0';
                        }
                        else
                        {
                            menuText[maxClipSize] = '\0';
                        }

                        for (unsigned int k = 0; k < maxClipSize; k++)
                        {
                            menuText[k] = clip[k];
                        }

                        AppendMenu(sMenu, MF_STRING, j + 1, menuText);
                    }
                }
                AppendMenu(menu, MF_STRING | MF_POPUP, (UINT_PTR)sMenu, L"More...");
            }
        }
        else
        {
            AppendMenu(menu, MF_STRING | MF_DISABLED, 0, L"Nothing on the clipboard");
        }

        AppendMenu(menu, MF_SEPARATOR, 0, 0);
        AppendMenu(menu, MF_STRING, CLEARCLIPS_SELECT, L"&Clear Clips");
        AppendMenu(menu, MF_SEPARATOR, 0, 0);
        AppendMenu(menu, MF_STRING, SETTINGS_SELECT, L"&Settings");
        if (showExit)
        {
            AppendMenu(menu, MF_STRING, EXIT_SELECT, L"&Exit");
        }
		

        int menuSelection;
        menuSelection = TrackPopupMenu(
            menu,
            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
            (*cPos).x,
            (*cPos).y,
            0,
            hWnd,
            NULL
        );

		if (menuSelection)
		{
            switch (menuSelection)
            {
            case CLEARCLIPS_SELECT:
                SendMessage(hWnd, WM_COMMAND, IDM_CLEARCLIPS, 0);
                SetActiveWindow(actWin);
                break;
            case SETTINGS_SELECT:
                PostMessage(hWnd, WM_COMMAND, IDM_SETTINGS, 0);
                break;
            case EXIT_SELECT:
                SendMessage(hWnd, WM_COMMAND, IDM_EXIT, 0);
                break;
            default:
                // for once a program where default actually does something more than error handling
                SetActiveWindow(actWin);
                Sleep(20);

                // Have to subtract 1 from index because returning 0 in
                // TrackPopupMenu means cancelation
                cm.SetClipboardToClipAtIndex(actWin, menuSelection - 1);
				
				// attempts to set the active window. Usually succeeds immediately, but not always 
				int retries = 10;
				HWND temp = GetActiveWindow();
				while (temp != actWin && retries > 0)
				{
					temp = GetActiveWindow();
					retries--;
					Sleep(20);
				}

                SendPasteInput();
                SetActiveWindow(actWin);
				break;
            }
		}
		// detatch from thread of the active window
		AttachThreadInput(GetWindowThreadProcessId(curWin, NULL), GetWindowThreadProcessId(hWnd, NULL), FALSE);

		PostMessage(hWnd, WM_NULL, 0, 0);
        delete[] menuText;
	}
}

void SelectDefaultMenuItem(bool select2ndClip)
{
    INPUT inputDownKey;

    inputDownKey.type = INPUT_KEYBOARD;
    inputDownKey.ki.wVk = VK_DOWN;
    inputDownKey.ki.wScan = 0;
    inputDownKey.ki.time = 0;
    inputDownKey.ki.dwFlags = 0;
    inputDownKey.ki.dwExtraInfo = 0;

    SendInput(1, &inputDownKey, sizeof(INPUT));
    Sleep(50);
    inputDownKey.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inputDownKey, sizeof(INPUT));

    if (select2ndClip)
    {
        inputDownKey.ki.dwFlags = 0;
        SendInput(1, &inputDownKey, sizeof(INPUT));
        Sleep(50);
        inputDownKey.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &inputDownKey, sizeof(INPUT));
    }
    return;
}

void SendPasteInput(void)
{
	INPUT inputCtrlKey;
	INPUT inputVKey;

	inputCtrlKey.type = INPUT_KEYBOARD;
	inputCtrlKey.ki.wVk = VK_CONTROL;
	inputCtrlKey.ki.wScan = 0;
	inputCtrlKey.ki.time = 0;
	inputCtrlKey.ki.dwFlags = 0;
	inputCtrlKey.ki.dwExtraInfo = 0;

	inputVKey.type = INPUT_KEYBOARD;
	inputVKey.ki.wVk = 0x56;
	inputVKey.ki.wScan = 0;
	inputVKey.ki.time = 0;
	inputVKey.ki.dwFlags = 0;
	inputVKey.ki.dwExtraInfo = 0;

    SendInput(1, &inputCtrlKey, sizeof(INPUT));
    SendInput(1, &inputVKey, sizeof(INPUT));

    Sleep(50);

    inputCtrlKey.ki.dwFlags = KEYEVENTF_KEYUP;
    inputVKey.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inputVKey, sizeof(INPUT));
    SendInput(1, &inputCtrlKey, sizeof(INPUT));

	return;
}

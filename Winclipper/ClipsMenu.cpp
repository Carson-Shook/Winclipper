#include "stdafx.h"
#include "resource.h"
#include <thread>
#include "UserSettings.h"
#include "File.h"
#include "Shlobj.h"
#include "ClipsMenu.h"

void ClipsManager::SaveClipsAsync()
{
    std::thread([&]() {IncrementClipsWriterDelay(&clipsWriterWaitCount, this); }).detach();
}

void ClipsManager::IncrementClipsWriterDelay(int * waitCount, ClipsManager * cs)
{
    (*waitCount)++;
    Sleep(5000);
    (*waitCount)--;

    if (*waitCount == 0)
    {
        cs->WriteClips();
    }
}

void ClipsManager::WriteClips()
{
    File::BinaryWriteDeque(clips, _T("C:\\Users\\Carson Shook\\AppData\\Local\\Winclipper\\Winclipper\\clips.dat"));
}

ClipsManager::ClipsManager()
{
    //TCHAR* tempClipsPath;
    //SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &tempClipsPath);
    //_tcsncat_s(fullClipsPath, MAX_PATH, tempClipsPath, _tcslen(tempClipsPath));
    //_tcsncat_s(fullClipsPath, MAX_PATH, clipsFilePath, _tcslen(clipsFilePath));
    //CoTaskMemFree(tempClipsPath);
}

ClipsManager::ClipsManager(int displayClips, int maxClips, int menuChars)
{
    SetDisplayClips(displayClips);
    SetMaxClips(maxClips);
    SetMenuDisplayChars(menuChars);
}

ClipsManager::~ClipsManager()
{
    ClearClips();
}


int ClipsManager::DisplayClips()
{
    return displayClips;
}

void ClipsManager::SetDisplayClips(int displayClips)
{
    if (displayClips != ClipsManager::displayClips)
    {
        ClipsManager::displayClips = displayClips;
    }
}

int ClipsManager::MaxClips()
{
    return maxClips;
}

void ClipsManager::SetMaxClips(int maxClips)
{
    if (maxClips != ClipsManager::maxClips)
    {
        ClipsManager::maxClips = maxClips;

        while (clips.size() > ClipsManager::maxClips)
        {
            // get reference to the back object before we pop it
            TCHAR * back = clips.back();
            clips.pop_back();
            // dealocate the former back object
            delete[] back;
        }
    }
}

int ClipsManager::MenuDisplayChars()
{
    return menuDispChars;
}

void ClipsManager::SetMenuDisplayChars(int menuDispChars)
{
    ClipsManager::menuDispChars = menuDispChars;
}

void ClipsManager::ClearClips()
{
    for (int i = 0, j = clips.size(); i < j; i++)
    {
        TCHAR * clip = clips.at(i);
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
        TCHAR * data = (TCHAR*)GlobalLock(psClipboardData);
        if (data != NULL)
        {
            TCHAR * derefData = _wcsdup(data);
            GlobalUnlock(psClipboardData);

            while (clips.size() >= maxClips)
            {
                // get reference to the back object before we pop it
                TCHAR * back = clips.back();
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

    TCHAR * clip = clips.at(index);

    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (_tcslen(clip) + 1) * sizeof(TCHAR));
    if (hClipboardData != NULL)
    {
        TCHAR * pwcData = (TCHAR*)GlobalLock(hClipboardData);
        if (pwcData != NULL)
        {
            wcscpy_s(pwcData, _tcslen(clip) + 1, clip);

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

void ShowClipsMenu(HWND hWnd, HWND curWin, ClipsManager& cm, bool showExit)
{
	if (hWnd != NULL && curWin != NULL)
	{
		SetForegroundWindow(hWnd);

		LPPOINT cPos = new POINT;
		if (!GetCursorPos(cPos))
		{
            (*cPos).x = 10;
            (*cPos).y = 10;
		}

		HMENU menu = CreatePopupMenu();

        int curSize = cm.GetClips().size();
        int displayCount = cm.DisplayClips();
        int menuTextLength = cm.MenuDisplayChars();
        TCHAR* menuText = new TCHAR[menuTextLength];

        if (curSize > 0)
        {
            int j = curSize > displayCount ? displayCount : curSize;

            for (int i = 0; i < j; i++)
            {
                TCHAR * clip = cm.GetClips().at(i);
                if (clip != NULL)
                {
                    int maxClipSize = _tcslen(clip);

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

                    for (int k = 0; k < maxClipSize; k++)
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
                    TCHAR * clip = cm.GetClips().at(j);
                    if (clip != NULL)
                    {
                        int maxClipSize = _tcslen(clip);

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

                        for (int k = 0; k < maxClipSize; k++)
                        {
                            menuText[k] = clip[k];
                        }

                        AppendMenu(sMenu, MF_STRING, j + 1, menuText);
                    }
                }
                AppendMenu(menu, MF_STRING | MF_POPUP, (UINT_PTR)sMenu, _T("More..."));
            }
        }
        else
        {
            AppendMenu(menu, MF_STRING | MF_DISABLED, 0, _T("Nothing on the clipboard"));
        }

        AppendMenu(menu, MF_SEPARATOR, 0, 0);
        AppendMenu(menu, MF_STRING, CLEARCLIPS_SELECT, _T("&Clear Clips"));
        AppendMenu(menu, MF_SEPARATOR, 0, 0);
        AppendMenu(menu, MF_STRING, SETTINGS_SELECT, _T("&Settings"));
        if (showExit)
        {
            AppendMenu(menu, MF_STRING, EXIT_SELECT, _T("&Exit"));
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
                SetForegroundWindow(curWin);
                break;
            case SETTINGS_SELECT:
                PostMessage(hWnd, WM_COMMAND, IDM_SETTINGS, 0);
                break;
            case EXIT_SELECT:
                SendMessage(hWnd, WM_COMMAND, IDM_EXIT, 0);
                break;
            default:
                // for once a program where default actually does something more than error handling
                SetForegroundWindow(curWin);
                Sleep(20);

                // Have to subtract 1 from index because returning 0 in
                // TrackPopupMenu means cancelation
                cm.SetClipboardToClipAtIndex(curWin, menuSelection - 1);

                Sleep(20);
                SendPasteInput();
                SetForegroundWindow(curWin);
                break;
            }
		}
		PostMessage(hWnd, WM_NULL, 0, 0);
        delete[] menuText;
	}
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

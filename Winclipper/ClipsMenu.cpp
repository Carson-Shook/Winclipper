#include "stdafx.h"
#include "ClipsMenu.h"

ClipsManager::ClipsManager()
{
}

ClipsManager::~ClipsManager()
{
    for (int i = 0, j = clips.size(); i < j; i++)
    {
        wchar_t * clip = clips.at(i);
        delete clip;
    }
    clips.clear();
}

BOOL ClipsManager::AddToClips(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
        return FALSE;
    HANDLE psClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (psClipboardData != NULL)
    {
        wchar_t * data = (wchar_t*)GlobalLock(psClipboardData);
        wchar_t * derefData = _wcsdup(data);
        GlobalUnlock(psClipboardData);

        if (clips.size() == maxClips)
        {
            // get reference to the back object before we pop it
            wchar_t * back = clips.back();
            clips.pop_back();
            // dealocate the former back object
            delete back;
        }
        clips.push_front(derefData);
    }
    CloseClipboard();
}

BOOL ClipsManager::SetClipboardToClipAtIndex(HWND, int)
{
    return 0;
}

int ShowClipsMenu(HWND hWnd, HWND curWin, ClipsManager& cm)
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

        for (int i = 0, j = cm.GetClips().size(); i < j; i++)
        {
            wchar_t * clip = cm.GetClips().at(i);
            if (clip != NULL)
            {
                int maxClipSize = wcslen(clip);

                wchar_t menuText[MENU_TEXT_LENGTH];

                // I know this is a little unorthodox, but we can count
                // on these lengths so long as MENU_TEXT_LENGTH > 5
                // and I really want these ellipses.
                if (maxClipSize > MENU_TEXT_LENGTH - 4)
                {
                    maxClipSize = MENU_TEXT_LENGTH - 4;
                    menuText[MENU_TEXT_LENGTH - 4] = '.';
                    menuText[MENU_TEXT_LENGTH - 3] = '.';
                    menuText[MENU_TEXT_LENGTH - 2] = '.';
                    menuText[MENU_TEXT_LENGTH - 1] = '\0';
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

        AppendMenu(menu, MF_SEPARATOR, 0, 0);
        AppendMenu(menu, MF_STRING, 1000, L"Settings");
		
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
			SetForegroundWindow(curWin);
			Sleep(10);
            
            if (!OpenClipboard(curWin))
            {
                return FALSE;
            }

            EmptyClipboard();

            wchar_t * clip = cm.GetClips().at(menuSelection - 1);

            HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (wcslen(clip) + 1) * sizeof(wchar_t));
			if (hClipboardData != NULL)
			{
				wchar_t * pwcData = (wchar_t*)GlobalLock(hClipboardData);
                wcscpy_s(pwcData, wcslen(clip) + 1, clip);

                GlobalUnlock(hClipboardData);

                SetClipboardData(CF_UNICODETEXT, hClipboardData);
			}
			CloseClipboard();
            Sleep(10);
            SendPasteInput();
		}
		SetForegroundWindow(curWin);
		PostMessage(hWnd, WM_NULL, 0, 0);
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

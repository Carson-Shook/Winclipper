#include "stdafx.h"
#include "ClipsManager.h"

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
    isWriterFinished = false;
    File::BinaryWriteDeque(clips, fullClipsPath);
    isWriterFinished = true;
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

void ClipsManager::SendPasteInput(void)
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

bool ClipsManager::NoPendingClipWrites()
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
    for (size_t i = 0; i < GetSize(); i++)
    {
        wchar_t * clip = clips.at(i);
        delete[] clip;
    }
    clips.clear();
    SaveClipsAsync();
}

bool ClipsManager::AddToClips(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
    {
        return false;
    }

    HANDLE psClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (psClipboardData != nullptr)
    {
        wchar_t * data = (wchar_t*)GlobalLock(psClipboardData);
        if (data != nullptr)
        {
            wchar_t * derefData = _wcsdup(data);
            GlobalUnlock(psClipboardData);

			if (clips.empty())
			{
				clips.push_front(derefData);
			}
			else
			{
				if (StrCmpW(derefData, clips.front()) != 0)
				{
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
			}
        }
        else 
        {
            GlobalUnlock(psClipboardData);
        }
    }
    CloseClipboard();
    SaveClipsAsync();
    return true;
}

bool ClipsManager::SetClipboardToClipAtIndex(HWND hWnd, int index)
{
    if (!OpenClipboard(hWnd))
    {
        return false;
    }

    EmptyClipboard();

    wchar_t * clip = clips.at(index);

    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (wcslen(clip) + 1) * sizeof(wchar_t));
    if (hClipboardData != nullptr)
    {
        wchar_t * pwcData = (wchar_t*)GlobalLock(hClipboardData);
        if (pwcData != nullptr)
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
    return true;
}

const size_t ClipsManager::GetSize() noexcept
{
	return clips.size();
}

// Guarantees to return a pointer to wchar_t 
// or nullptr if clip doesn't exist at index
wchar_t * ClipsManager::GetClipAt(size_t index) noexcept
{
	try
	{
#pragma warning( suppress : 26447 )
		return clips.at(index);
	}
	catch (const std::exception e)
	{
		return nullptr;
	}
}

void ClipsManager::ShowClipsMenu(HWND hWnd, LPPOINT cPos, bool showExit)
{
    HWND curWin = GetForegroundWindow();
	HWND taskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
	HWND child = GetWindow(curWin, GW_CHILD);

	// Ensures that the last *real* active window is selected
	// when the user tries to paste using the shell_notifyicon.
	if (curWin == taskbarWnd || IsChild(taskbarWnd, curWin))
	{
		curWin = taskbarWnd; // just in case isChild instead of equal.
		do
		{
			curWin = GetWindow(curWin, GW_HWNDNEXT);
		} while (!IsWindowVisible(curWin));
	}
	else if (child != nullptr)
	{
		// Microsoft Edge hack.
		// If a child window looks like it belongs to MS Edge,
		// then search previous from curWin until we get to a
		// visible window. That should be the one we care about.
		child = FindWindowExW(curWin, nullptr, L"Windows.UI.Core.CoreWindow", L"Microsoft Edge");
		if (child != nullptr)
		{
			do
			{
				curWin = GetWindow(curWin, GW_HWNDPREV);
			} while (!IsWindowVisible(curWin));
		}
	}

    if (hWnd != nullptr && curWin != nullptr)
    {
		SetForegroundWindow(curWin);

        // Since WindowsNT (I think), it's necessary to attach the process to the thread
        // that we want to get the active window of, otherwise we get an unspecified value.
        // GetForegroundWindow was unreliable with UWP applications because it took too long
        // to restore the active window (the text field) after the foreground window activated.
        AttachThreadInput(GetWindowThreadProcessId(curWin, nullptr), GetWindowThreadProcessId(hWnd, nullptr), true);
		HWND actWin = GetActiveWindow();

        SetForegroundWindow(hWnd);

        HMENU menu = CreatePopupMenu();

        const size_t curSize = GetSize();

        if (curSize > 0)
        {
            size_t j = curSize > DisplayClips() ? DisplayClips() : curSize;

            for (size_t i = 0; i < j; i++)
            {
                const wchar_t * clip = GetClipAt(i);
                if (clip != nullptr)
                {
                    const size_t clipSize = wcslen(clip);
					
					std::wstring menuItemText;

					if (clipSize > MenuDisplayChars())
					{
						menuItemText.assign(clip, 0, MenuDisplayChars() - 3);
						menuItemText.append(L"...\0");
					}
					else
					{
						menuItemText.assign(clip);
					}

					for (size_t k = 0; k < menuItemText.size(); k++)
					{
						switch (menuItemText.at(k))
						{
						case '\t':
						case '\r':
						case '\n':
							menuItemText.at(k) = ' ';
						}
					}

					AppendMenuW(menu, MF_STRING, UINT_PTR{ i } + 1, menuItemText.c_str());
                }
            }
            if (curSize > DisplayClips())
            {
                HMENU sMenu = CreatePopupMenu();
                
                for (/*j is equal to displayCount */; j < curSize; j++)
                {
                    const wchar_t * clip = GetClipAt(j);
                    if (clip != nullptr)
                    {
						const size_t clipSize = wcslen(clip);

						std::wstring menuItemText;

						if (clipSize > MenuDisplayChars())
						{
							menuItemText.assign(clip, 0, MenuDisplayChars() - 3);
							menuItemText.append(L"...\0");
						}
						else
						{
							menuItemText.assign(clip);
						}

						for (size_t k = 0; k < menuItemText.size(); k++)
						{
							switch (menuItemText.at(k))
							{
							case '\t':
							case '\r':
							case '\n':
								menuItemText.at(k) = ' ';
							}
						}

						AppendMenuW(sMenu, MF_STRING, UINT_PTR{ j } + 1, menuItemText.c_str());
                    }
                }
                AppendMenuW(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(sMenu), L"More...");
            }
        }
        else
        {
            AppendMenuW(menu, MF_STRING | MF_DISABLED, 0, L"Nothing on the clipboard");
        }

        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, CLEARCLIPS_SELECT, L"&Clear Clips");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, SETTINGS_SELECT, L"&Settings");
		// Debug info
		//wchar_t str[128];
		//wsprintf(str, L"Window handle is 0x%08x", curWin);
		//AppendMenuW(menu, MF_STRING, SETTINGS_SELECT, str);
        if (showExit)
        {
			AppendMenuW(menu, MF_STRING, EXIT_SELECT, L"&Exit");
        }
        

        int menuSelection;
        menuSelection = TrackPopupMenu(
            menu,
            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
            cPos->x,
            cPos->y,
            0,
            hWnd,
            NULL
        );
		// apparently this line is needed to ensure
		// consistent popup menu behavior.
		PostMessageW(hWnd, WM_USER, 0, 0);
        switch (menuSelection)
        {
		case CANCELED_SELECTION:
			{
				SetActiveWindow(actWin); 
				int retries = 10;
				HWND temp = GetActiveWindow();
				while (temp != actWin && retries > 0)
				{
					temp = GetActiveWindow();
					retries--;
					Sleep(20);
				}
			}
			break;
        case CLEARCLIPS_SELECT:
            PostMessageW(hWnd, WM_COMMAND, IDM_CLEARCLIPS, 0);
            SetActiveWindow(actWin);
            break;
        case SETTINGS_SELECT:
            PostMessageW(hWnd, WM_COMMAND, IDM_SETTINGS, 0);
            break;
        case EXIT_SELECT:
			PostMessageW(hWnd, WM_COMMAND, IDM_EXIT, 0);
            break;
        default:
            // for once a program where default actually does something more than error handling
			SetForegroundWindow(curWin);
            SetActiveWindow(actWin);
            Sleep(20);

            // Have to subtract 1 from index because returning 0 in
            // TrackPopupMenu means cancelation
            SetClipboardToClipAtIndex(actWin, menuSelection - 1);
                
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

        // detatch from thread of the active window
        AttachThreadInput(GetWindowThreadProcessId(curWin, nullptr), GetWindowThreadProcessId(hWnd, nullptr), false);

        PostMessageW(hWnd, WM_NULL, 0, 0);
    }
}

void ClipsManager::SelectDefaultMenuItem(bool select2ndClip) noexcept
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
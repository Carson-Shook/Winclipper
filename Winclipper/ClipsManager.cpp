#include "stdafx.h"
#include "wincodec.h"
#include "ClipsManager.h"

void ClipsManager::SaveClipsAsync()
{
    if (UserSettings::SaveToDisk())
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
        if (File::Exists(fullClipsPath.c_str()))
        {
            File::Delete(fullClipsPath.c_str());
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
	try
	{
		File::Write(fullClipsPath, clips.Serialize());
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Winclipper", MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND);
	}
    isWriterFinished = true;
}

void ClipsManager::ReadClips()
{
    if (File::Exists(fullClipsPath.c_str()))
    {
		try
		{
			clips.Deserialize(File::Read(fullClipsPath));
		}
		catch (const std::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "Winclipper", MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND);
		}
    }
}

unsigned int ClipsManager::SendPasteInput(void) noexcept
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

	const unsigned int in1 = SendInput(1, &inputCtrlKey, sizeof(INPUT));
	const unsigned int in2 = SendInput(1, &inputVKey, sizeof(INPUT));

	Sleep(50);

	inputCtrlKey.ki.dwFlags = KEYEVENTF_KEYUP;
	inputVKey.ki.dwFlags = KEYEVENTF_KEYUP;
	const unsigned int in3 = SendInput(1, &inputVKey, sizeof(INPUT));
	const unsigned int in4 = SendInput(1, &inputCtrlKey, sizeof(INPUT));

	return in1 + in2 + in3 + in4;
}

void ClipsManager::ClearBitmaps()
{
	clips.RemoveAllOfFormat(CF_DIB);
}

ClipsManager::ClipsManager()
{
	fullClipsPath = File::JoinPath(File::GetAppDir(), L"clips.dat");

	windows10ReleaseId = RegistryUtilities::GetWindows10ReleaseId();

	ReadClips();
    UpdateMaxClips();
	UpdateSavedImages();
	SaveClipsAsync();
}

ClipsManager::~ClipsManager()
{
}

bool ClipsManager::NoPendingClipWrites() noexcept
{
    return clipsWriterWaitCount == 0 && isWriterFinished;
}


void ClipsManager::UpdateMaxClips()
{
	clips.SetMaxSize(UserSettings::MaxSavedClips());
    SaveClipsAsync();
}

void ClipsManager::SetSaveToDisk(bool saveToDisk)
{
	OnDemandBitmapManager::SetSaveToDisk(saveToDisk);

	if (!UserSettings::SaveToDisk())
	{
		clips.LoadAllResources();
		OnDemandBitmapManager::DeleteAllFromDisk();
	}
    SaveClipsAsync();
}

void ClipsManager::UpdateSavedImages()
{
	if (UserSettings::SaveImages())
	{
		ClearBitmaps();
		SaveClipsAsync();
	}
}

void ClipsManager::RecreateThumbnails()
{
	clips.PurgeThumbnails();
}

void ClipsManager::ClearClips()
{
	clips.Clear();
    SaveClipsAsync();
}

bool ClipsManager::AddToClips(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
    {
        return false;
    }

	std::shared_ptr<Clip> clip = std::make_shared<Clip>();

	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		HANDLE psClipboardData = GetClipboardData(CF_UNICODETEXT);
		if (psClipboardData != nullptr)
		{
			const wchar_t * data = static_cast<wchar_t*>(GlobalLock(psClipboardData));
			if (data != nullptr)
			{
				clip->SetUnicodeText(data);
				GlobalUnlock(psClipboardData);
				clip->AddFormat(CF_UNICODETEXT);
			}
			else
			{
				GlobalUnlock(psClipboardData);
			}
		}
	}

	if (UserSettings::SaveImages() && IsClipboardFormatAvailable(CF_DIB))
	{
		bool hasTransparentBitmapFormat = false;

		UINT currentFormat = 0;
		do
		{
			currentFormat = EnumClipboardFormats(currentFormat);
			// If CF_BITMAP is listed before the other better formats, then we can assume
			// that it was "real" format and the others are synthesized.
			if (currentFormat == CF_BITMAP)
			{
				currentFormat = 0;
			}
			hasTransparentBitmapFormat = currentFormat == CF_DIBV5 || currentFormat == CF_DIB || currentFormat == CF_PNG;
		} while (currentFormat != 0 && !hasTransparentBitmapFormat);

		HANDLE psClipboardData = GetClipboardData(CF_DIB);

		if (psClipboardData != nullptr)
		{
			const BITMAPINFO * tempBitmapInfo = static_cast<BITMAPINFO *>(GlobalLock(psClipboardData));

			// See: https://docs.microsoft.com/en-us/windows/desktop/api/d2d1/nf-d2d1-id2d1rendertarget-getmaximumbitmapsize
			if (tempBitmapInfo != nullptr && tempBitmapInfo->bmiHeader.biHeight < 16384 && tempBitmapInfo->bmiHeader.biWidth < 16384)
			{
				unsigned int colorBytes = tempBitmapInfo->bmiHeader.biClrUsed;
				if (!colorBytes)
				{
					if (tempBitmapInfo->bmiHeader.biBitCount <= 8)
					{
						colorBytes = 1 << tempBitmapInfo->bmiHeader.biBitCount;
					}
					else if (tempBitmapInfo->bmiHeader.biBitCount != 24 && tempBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS)
					{
						colorBytes = 3;
					}
				}

				const size_t offset = tempBitmapInfo->bmiHeader.biSize + (colorBytes * sizeof(RGBQUAD));

				std::shared_ptr<BITMAPINFOHEADER> bitmapInfoHeader = std::make_shared<BITMAPINFOHEADER>();
				std::vector<RGBQUAD> quads;

				*bitmapInfoHeader = tempBitmapInfo->bmiHeader;

				if (bitmapInfoHeader->biBitCount != 24)
				{
					for (unsigned int i = 0; i < colorBytes; i++)
					{
						const RGBQUAD quad = tempBitmapInfo->bmiColors[i];
						quads.push_back(quad);
					}
				}

				// If not zero, use biSizeImage, otherwise calculate it using
				// Microsoft's reccomended algorithm.
				const DWORD size = bitmapInfoHeader->biSizeImage 
					? bitmapInfoHeader->biSizeImage 
					: ((((bitmapInfoHeader->biWidth * bitmapInfoHeader->biBitCount) + 31) & ~31) >> 3) * bitmapInfoHeader->biHeight;

				std::shared_ptr<BYTE> pMyBits(new BYTE[size], array_deleter<BYTE>());
				memcpy_s(pMyBits.get(), size, (reinterpret_cast<const BYTE *>(tempBitmapInfo)) + offset, size);

				if (bitmapInfoHeader->biBitCount == 32 && !hasTransparentBitmapFormat)
				{
					for (DWORD i = 3; i < size - 1; i += 4)
					{
						pMyBits.get()[i] = 255;
					}
				}

				GlobalUnlock(psClipboardData);

				try
				{
					clip->SetDibBitmap(
						bitmapInfoHeader,
						quads,
						pMyBits
					);
					clip->AddFormat(CF_DIB);
				}
				catch (const std::exception&)
				{
					// Not really sure I should put up a message if this fails
				}
			}
			else
			{
				GlobalUnlock(psClipboardData);
				
			}
		}
	}

	CloseClipboard();

	if (clip->AnyFormats())
	{
		clips.AddFront(clip);
	}
	else
	{
		clip->MarkForDelete();
	}
    SaveClipsAsync();
    return true;
}

bool ClipsManager::SetClipboardToClipAtIndex(HWND hWnd, unsigned int index)
{
    if (!OpenClipboard(hWnd))
    {
        return false;
    }

    EmptyClipboard();

	std::shared_ptr<Clip> clip = index == 0 ? GetClipAt(index) : clips.RemoveAt(index);

	if (clip != nullptr)
	{
		if (clip->ContainsFormat(CF_UNICODETEXT))
		{
			size_t clipLength = clip->UnicodeTextWString().length();

			HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (clipLength + 1) * sizeof(wchar_t));
			if (hClipboardData != nullptr)
			{
				wchar_t * pwcData = static_cast<wchar_t*>(GlobalLock(hClipboardData));
				if (pwcData != nullptr)
				{
					wcscpy_s(pwcData, clipLength + 1, clip->UnicodeText());

					GlobalUnlock(hClipboardData);

					SetClipboardData(CF_UNICODETEXT, hClipboardData);
				}
				else
				{
					GlobalUnlock(hClipboardData);
				}
			}
		}

		if (clip->ContainsFormat(CF_DIB))
		{
			auto bmiHeader = clip->DibBitmapInfoHeader();
			auto quadCollection = clip->RgbQuadCollection();
			const size_t rgbQuadsCount = quadCollection.size();
			const DWORD bitmapSize = clip->DibSize();

			HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, bmiHeader->biSize + rgbQuadsCount * sizeof(RGBQUAD) + bitmapSize);

			if (hClipboardData != nullptr && bmiHeader != nullptr)
			{
				BITMAPINFO * bmi = static_cast<BITMAPINFO *>(GlobalLock(hClipboardData));
				
				if (bmi != nullptr)
				{
					bmi->bmiHeader = *bmiHeader;

					for (size_t i = 0; i < rgbQuadsCount; i++)
					{
						bmi->bmiColors[i] = quadCollection[i];
					}

					memcpy_s(reinterpret_cast<BYTE *>(bmi) + (bmiHeader->biSize + rgbQuadsCount * sizeof(RGBQUAD)), bitmapSize, clip->DibBitmapBits().get(), bitmapSize);

					GlobalUnlock(hClipboardData);
					
					if (!SetClipboardData(CF_DIB, hClipboardData))
					{
						GlobalFree(hClipboardData);
					}
				}
				else
				{
					GlobalUnlock(hClipboardData);
				}
			}
		}

		if (index != 0)
		{
			clips.AddFront(clip);
		}
	}
    CloseClipboard();
    SaveClipsAsync();
    return true;
}

const unsigned int ClipsManager::GetSize() noexcept
{
	return clips.Size();
}

void ClipsManager::DeleteClipAt(unsigned int index)
{
	auto clip = clips.RemoveAt(index);
	clip->MarkForDelete();
	SaveClipsAsync();
}

std::shared_ptr<Clip> ClipsManager::GetClipAt(unsigned int index)
{
	try
	{
		return clips.At(index);
	}
	catch (const std::exception e)
	{
		return nullptr;
	}
}

void ClipsManager::ShowClipsMenu(HWND hWnd, const LPPOINT cPos, bool showExit)
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
		if (windows10ReleaseId > 0 && windows10ReleaseId < 1809)
		{
			// Microsoft Edge hack.
			// (Note: as of 1809, this doesn't appear to be necessary)
			// If a child window looks like it belongs to MS Edge,
			// then search previous from curWin until we get to a
			// visible window. That should be the one we care about.
			child = FindWindowExW(curWin, nullptr, L"Windows.UI.Core.CoreWindow", L"Microsoft Edge");
			if (child != nullptr)
			{
				do
				{
					curWin = GetWindow(curWin, GW_CHILD);
				} while (!IsWindowVisible(curWin));
			}
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

        const unsigned int curSize = GetSize();

        if (curSize > 0)
        {
            unsigned int j = curSize > UserSettings::MaxDisplayClips() ? UserSettings::MaxDisplayClips() : curSize;

            for (unsigned int i = 0; i < j; i++)
            {
                std::shared_ptr<Clip> clip = GetClipAt(i);
                if (clip != nullptr)
                {
					if (clip->ContainsFormat(CF_UNICODETEXT))
					{
						std::wstring menuText;
						menuText = clip->GetUnicodeMenuText(UserSettings::MenuDisplayChars());

						AppendMenuW(menu, MF_STRING, UINT_PTR{ i } +1, menuText.c_str());
						if (clip->ContainsFormat(CF_DIB))
						{
							SetMenuItemBitmaps(menu, UINT{ i } +1, MF_BYCOMMAND, clip->GetThumbnail(), clip->GetThumbnail());
						}
					}
					else if (clip->ContainsFormat(CF_DIB))
					{
						AppendMenuW(menu, MF_STRING, UINT_PTR{ i } + 1, (L"[IMAGE] " + std::to_wstring(clip->DibHeight()) + L" x " + std::to_wstring(clip->DibWidth())).c_str());
						SetMenuItemBitmaps(menu, UINT{ i } + 1, MF_BYCOMMAND, clip->GetThumbnail(), clip->GetThumbnail());
					}
                }
            }
            if (curSize > UserSettings::MaxDisplayClips())
            {
                HMENU sMenu = CreatePopupMenu();
                
                for (/*j is equal to displayCount */; j < curSize; j++)
                {
					std::shared_ptr<Clip> clip = GetClipAt(j);
                    if (clip != nullptr)
                    {
						if (clip->ContainsFormat(CF_UNICODETEXT))
						{
							std::wstring menuText;
							menuText = clip->GetUnicodeMenuText(UserSettings::MenuDisplayChars());

							AppendMenuW(sMenu, MF_STRING, UINT_PTR{ j } + 1, menuText.c_str());
							if (clip->ContainsFormat(CF_DIB))
							{
								SetMenuItemBitmaps(sMenu, UINT{ j } + 1, MF_BYCOMMAND, clip->GetThumbnail(), clip->GetThumbnail());
							}
						}
						else if (clip->ContainsFormat(CF_DIB))
						{
							AppendMenuW(sMenu, MF_STRING, UINT_PTR{ j } + 1, (L"[IMAGE] " + std::to_wstring(clip->DibHeight()) + L" x " + std::to_wstring(clip->DibWidth())).c_str());
							SetMenuItemBitmaps(sMenu, UINT{ j } + 1, MF_BYCOMMAND, clip->GetThumbnail(), clip->GetThumbnail());
						}
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
            cPos->x - 5,
            cPos->y - 5,
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
			SetForegroundWindow(curWin);
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
			SetForegroundWindow(curWin);
			SetActiveWindow(actWin);
            break;
        case SETTINGS_SELECT:
            PostMessageW(hWnd, WM_COMMAND, IDM_SETTINGS, 0);
            break;
        case EXIT_SELECT:
			PostMessageW(hWnd, WM_COMMAND, IDM_EXIT, 0);
            break;
        default:
			ClipLock = true;

			// Have to subtract 1 from index because returning 0 in
			// TrackPopupMenu means cancelation
			int retries = 10;
			bool success = SetClipboardToClipAtIndex(actWin, menuSelection - 1);
			while (!success && retries > 0)
			{
				Sleep(100);
				success = SetClipboardToClipAtIndex(actWin, menuSelection - 1);
				retries--;
			}

			// Attempts to set the active window. Usually succeeds immediately, but not always 
			SetForegroundWindow(curWin);
			SetActiveWindow(actWin);
			retries = 10;
			HWND temp = GetActiveWindow();
			while (temp != actWin && retries > 0)
			{
				Sleep(100);
				temp = GetActiveWindow();
				retries--;
			}

			retries = 10;
			unsigned int inputCount = SendPasteInput();
			while (inputCount != 4 && retries > 0)
			{
				Sleep(100);
				inputCount = SendPasteInput();
				retries--;
			}
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

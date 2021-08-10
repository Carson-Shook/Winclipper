#include "stdafx.h"
#include "SettingsWindow.h"


ATOM SettingsWindow::RegisterSettingsWindowClass(HINSTANCE hInstance)
{
	useLightTheme = RegistryUtilities::AppsUseLightTheme();

	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = SettingsWindow::SettingsWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	if (useLightTheme)
	{
		wcex.hbrBackground = APP_BACKGROUND_COLOR_LIGHT;
	}
	else
	{
		wcex.hbrBackground = APP_BACKGROUND_COLOR_DARK;
	}
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINCSETTINGS);
	wcex.lpszClassName = szSettingsWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));

	return RegisterClassExW(&wcex);
}

LRESULT SettingsWindow::SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// We want to capture a pointer to the current instance of the class
	// so we can call non-static methods from a static context.
	SettingsWindow * pThis = nullptr;

	if (message == WM_NCCREATE)
	{
#pragma warning( suppress : 26490 )
		pThis = static_cast<SettingsWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
#pragma warning( suppress : 26490 )
		if (!SetWindowLongPtrW(hWnd, AGNOSTIC_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
		return TRUE;
	}
	else
	{
#pragma warning( suppress : 26490 )
		pThis = reinterpret_cast<SettingsWindow*>(GetWindowLongPtrW(hWnd, AGNOSTIC_USERDATA));
	}

	if (!pThis)
	{
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	LRESULT lResult = 0;

	switch (message)
	{
	case WM_COMMAND:
	{
		const int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			lResult = pThis->WmCommandIdmAbout(hWnd, wParam, lParam);
			break;
		case IDM_EXIT:
			lResult = pThis->WmCommandIdmExit();
			break;
		case IDM_RECREATE_THUMBNAILS:
			lResult = pThis->WmCommandIdmRecreateThumbnails();
			break;
		case IDM_CLEARCLIPS:
			lResult = pThis->WmCommandIdmClearClips();
			break;
		case TXT_MAX_CLIPS_DISPLAY:
			lResult = pThis->WmCommandTxtMaxClipsDisplay(hWnd, wParam, lParam);
			break;
		case TXT_MAX_CLIPS_SAVED:
			lResult = pThis->WmCommandTxtMaxClipsSaved(hWnd, wParam, lParam);
			break;
		case TXT_MENU_DISP_CHARS:
			lResult = pThis->WmCommandTxtMenuDispChars(hWnd, wParam, lParam);
			break;
		case CHK_RUN_AT_STARTUP:
			lResult = pThis->WmCommandChkRunAtStartup(hWnd, wParam, lParam);
			break;
		case CHK_SAVE_TO_DISK:
			lResult = pThis->WmCommandChkSaveToDisk(hWnd, wParam, lParam);
			break;
		case CHK_SLCT_2ND_CLIP:
			lResult = pThis->WmCommandChkSlct2ndClip(hWnd, wParam, lParam);
			break;
		case CHK_SHOW_PREVIEW:
			lResult = pThis->WmCommandChkShowPreview(hWnd, wParam, lParam);
			break;
		case CHK_SAVE_IMAGES:
			lResult = pThis->WmCommandChkSaveImages(hWnd, wParam, lParam);
			break;
		case HKY_SHOW_CLIPS_MENU:
			lResult = pThis->WmCommandHkyShowClipsMenu(hWnd, wParam, lParam);
			break;
		case TXT_MAX_IMG_CACHE_MB:
			lResult = pThis->WmCommandTxtMaxImageCacheMb(hWnd, wParam, lParam);
			break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_LBUTTONDOWN:
		lResult = pThis->WmLbuttondown(hWnd);
		break;
	case WM_CTLCOLORSTATIC:
		lResult = pThis->WmCtlColorStatic(hWnd, wParam, lParam);
		break;
	case WM_CTLCOLOREDIT:
		lResult = pThis->WmCtlColorEdit(hWnd, wParam, lParam);
		break;
	case WM_CLOSE:
		lResult = pThis->WmClose(hWnd);
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return lResult;
}

LRESULT SettingsWindow::WmCommandIdmAbout(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SettingsWindow::SendNotifcation(IDM_ABOUT, GetHandle());
	return 0;
}

LRESULT SettingsWindow::WmCommandIdmExit()
{
	SettingsWindow::SendNotifcation(IDM_EXIT, GetHandle());
	return 0;
}

LRESULT SettingsWindow::WmCommandIdmRecreateThumbnails()
{
	SettingsWindow::SendNotifcation(IDM_RECREATE_THUMBNAILS, GetHandle());
	return 0;
}

LRESULT SettingsWindow::WmCommandIdmClearClips()
{
	SettingsWindow::SendNotifcation(IDM_CLEARCLIPS, GetHandle());
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMaxClipsDisplay(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		const int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, nullptr, false);

		if (value < UserSettings::MAX_DISPLAY_LOWER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, UserSettings::MAX_DISPLAY_LOWER, false);
		}
		else if (value > UserSettings::MAX_DISPLAY_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, UserSettings::MAX_DISPLAY_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		const int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, nullptr, false);

		if (value >= UserSettings::MAX_DISPLAY_LOWER && value <= UserSettings::MAX_DISPLAY_UPPER)
		{
			UserSettings::SetMaxDisplayClips(value);
			if (value > UserSettings::MaxSavedClips())
			{
				SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, value, false);
			}
			SendDlgItemMessageW(hWnd, UD_MAX_CLIPS_SAVED, UDM_SETRANGE, 0, MAKELPARAM(UserSettings::MAX_SAVED_UPPER, value));
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMaxClipsSaved(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		const int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, nullptr, false);

		if (value < UserSettings::MaxDisplayClips())
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, UserSettings::MaxDisplayClips(), false);
		}
		else if (value > UserSettings::MAX_SAVED_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, UserSettings::MAX_SAVED_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		const int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, nullptr, false);

		if (value <= UserSettings::MAX_SAVED_UPPER && value >= UserSettings::MaxDisplayClips())
		{
			UserSettings::SetMaxSavedClips(GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, nullptr, false));
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMenuDispChars(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		const int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, nullptr, false);

		if (value < UserSettings::MENU_CHARS_LOWER)
		{
			SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, UserSettings::MENU_CHARS_LOWER, false);
		}
		else if (value > UserSettings::MENU_CHARS_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, UserSettings::MENU_CHARS_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		const int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, nullptr, false);

		if (value >= UserSettings::MENU_CHARS_LOWER && value <= UserSettings::MENU_CHARS_UPPER)
		{
			UserSettings::SetMenuDisplayChars(value);
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkRunAtStartup(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		bool checkValue = CheckboxRunAtStartup.Value();
		if (checkValue)
		{
			DeleteRegistryRun();
		}
		else
		{
			WriteRegistryRun();
		}
		CheckboxRunAtStartup.SetValue(!checkValue);
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkSaveToDisk(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		bool checkValue = CheckboxSaveToDisk.Value();
		UserSettings::SetSaveToDisk(!checkValue);
		CheckboxSaveToDisk.SetValue(!checkValue);
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkSlct2ndClip(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		bool checkValue = CheckboxSelectSecondClip.Value();
		UserSettings::SetSelect2ndClip(!checkValue);
		CheckboxSelectSecondClip.SetValue(!checkValue);
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkShowPreview(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		bool checkValue = CheckboxShowPreview.Value();
		UserSettings::SetShowPreview(!checkValue);
		CheckboxShowPreview.SetValue(!checkValue);
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkSaveImages(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		bool checkValue = CheckboxSaveImages.Value();

		if (checkValue)
		{
			int choice = MessageBoxA(hWnd, 
				"This will delete all images on the Winclipper clipboard.\r\n\r\n Are you sure you want to do this?", "Winclipper", 
				MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TASKMODAL | MB_SETFOREGROUND);

			if (choice == IDYES)
			{
				UserSettings::SetSaveImages(!checkValue);
				CheckboxSaveImages.SetValue(!checkValue);
				SpinnerMaxCacheMbytes.SetEnabled(!checkValue);
			}
		}
		else
		{
			UserSettings::SetSaveImages(!checkValue);
			CheckboxSaveImages.SetValue(!checkValue);
			SpinnerMaxCacheMbytes.SetEnabled(!checkValue);;
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMaxImageCacheMb(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		const unsigned int value = GetDlgItemInt(hWnd, TXT_MAX_IMG_CACHE_MB, nullptr, false);

		if (value < UserSettings::MAX_CACHE_MBYTES_LOWER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_IMG_CACHE_MB, UserSettings::MAX_CACHE_MBYTES_LOWER, false);
		}
		else if (value > UserSettings::MAX_CACHE_MBYTES_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_IMG_CACHE_MB, UserSettings::MAX_CACHE_MBYTES_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		const unsigned int value = GetDlgItemInt(hWnd, TXT_MAX_IMG_CACHE_MB, nullptr, false);

		if (value >= UserSettings::MAX_CACHE_MBYTES_LOWER && value <= UserSettings::MAX_CACHE_MBYTES_UPPER)
		{
			UserSettings::SetMaxCacheMegabytes(value);
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandHkyShowClipsMenu(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_CHANGE)
	{
		WORD wHotkey;

		// Retrieve the hot key (virtual key code and modifiers). 
		wHotkey = (WORD)SendMessageW(GetDlgItem(hWnd, HKY_SHOW_CLIPS_MENU), HKM_GETHOTKEY, 0, 0);

		// Resets to the default hotkey if the combo is cleared.
		if (wHotkey == 0)
		{
			SendMessageW(GetDlgItem(hWnd, HKY_SHOW_CLIPS_MENU), HKM_SETHOTKEY, CMENU_HOTKEY_DEF, 0);
			UserSettings::SetClipsMenuHotkey(CMENU_HOTKEY_DEF);
		}
		else
		{
			// So the WORD returned from HKM_GETHOTKEY stuffs all of the
			// hotkey info in the LOWORD side into the LOBYTE and HIBYTE
			// intead of on each side of the WORD, so we have to split
			// these out for consistency with storing in settings.
			UserSettings::SetClipsMenuHotkey(MAKEWORD(LOBYTE(LOWORD(wHotkey)), HIBYTE(LOWORD(wHotkey))));
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmLbuttondown(HWND hWnd)
{
	SetFocus(hWnd);
	return 0;
}

LRESULT SettingsWindow::WmCtlColorStatic(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdcStatic = (HDC)wParam;
	SetBkMode(hdcStatic, TRANSPARENT);
	if (useLightTheme)
	{
		SetTextColor(hdcStatic, FOREGROUND_COLOR_LIGHT);
	}
	else
	{
		SetTextColor(hdcStatic, FOREGROUND_COLOR_DARK);
	}

	if (hbrBkgnd == NULL)
	{
		if (useLightTheme)
		{
			hbrBkgnd = CreateSolidBrush(BACKGROUND_COLOR_LIGHT);
		}
		else
		{
			hbrBkgnd = CreateSolidBrush(BACKGROUND_COLOR_DARK);
		}
	}
	return (INT_PTR)hbrBkgnd;
}

LRESULT SettingsWindow::WmCtlColorEdit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdcStatic = (HDC)wParam;
	SetBkMode(hdcStatic, TRANSPARENT);
	if (useLightTheme)
	{
		SetTextColor(hdcStatic, FOREGROUND_COLOR_LIGHT);
	}
	else
	{
		SetTextColor(hdcStatic, FOREGROUND_COLOR_DARK);
	}

	if (hbrCtrlBkgnd == NULL)
	{
		if (useLightTheme)
		{
			hbrCtrlBkgnd = CreateSolidBrush(BACKGROUND_COLOR_LIGHT);
		}
		else
		{
			hbrCtrlBkgnd = CreateSolidBrush(BACKGROUND_COLOR_DARK);
		}
	}
	return (INT_PTR)hbrCtrlBkgnd;
}

LRESULT SettingsWindow::WmClose(HWND hWnd)
{
	SetFocus(hWnd);
	ShowWindow(hWnd, SW_HIDE);
	return 0;
}

// Writes the "run at startup" registry key for Winclipper
bool SettingsWindow::WriteRegistryRun()
{
	HKEY hOpened;
	wchar_t pPath[MAX_PATH];

	GetModuleFileNameW(nullptr, pPath, MAX_PATH);

	//OpenRegistryRun(hOpened);
	RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened);

	if (RegSetValueExW(hOpened, L"Winclipper", 0, REG_SZ, (LPBYTE)pPath, sizeof(pPath)) != ERROR_SUCCESS)
	{
		return false;
	}

	RegCloseKey(hOpened);
	return true;
}

// Deletes the "run at startup" registry key for Winclipper
bool SettingsWindow::DeleteRegistryRun()
{
	HKEY hOpened;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened) == ERROR_SUCCESS)
	{
		RegDeleteValueW(hOpened, L"Winclipper");
	}
	else
	{
		return false;
	}

	RegCloseKey(hOpened);
	return true;
}

SettingsWindow::SettingsWindow(HINSTANCE hInstance)
{
	NONCLIENTMETRICS hfDefault;
	hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
	hFontStd = CreateFontIndirectW(&hfDefault.lfMessageFont);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINCSETTINGS, szSettingsWindowClass, MAX_LOADSTRING);

	RegisterSettingsWindowClass(hInstance);

	InitSettingsWindow(hInstance);
}

SettingsWindow::~SettingsWindow()
{
}

bool SettingsWindow::InitSettingsWindow(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindowExW(0, szSettingsWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, ScaleX(260), ScaleY(340), nullptr, nullptr, hInstance, this);

	if (!hWnd)
	{
		return false;
	}

	SetWindowTextW(hWnd, szTitle);

	windowHandle = hWnd;
	
	// Add controls in tab order
	LabelMaxClipsDisplay = Controls::Label(hWnd, LBL_MAX_CLIPS_DISPLAY, hInstance);
	LabelMaxClipsDisplay.SuspendLayout();
	LabelMaxClipsDisplay.SetX(10);
	LabelMaxClipsDisplay.SetY(10);
	LabelMaxClipsDisplay.SetText(L"Number of clips to display:");
	LabelMaxClipsDisplay.ResumeLayout();

	SpinnerMaxClipsDisplay = Controls::Spinner(hWnd, TXT_MAX_CLIPS_DISPLAY, UD_MAX_CLIPS_DISPLAY, hInstance);
	SpinnerMaxClipsDisplay.SuspendLayout();
	SpinnerMaxClipsDisplay.SetX(180);
	SpinnerMaxClipsDisplay.SetY(10);
	SpinnerMaxClipsDisplay.SetMinValue(UserSettings::MAX_DISPLAY_LOWER);
	SpinnerMaxClipsDisplay.SetMaxValue(UserSettings::MAX_DISPLAY_UPPER);
	SpinnerMaxClipsDisplay.ResumeLayout();

	LabelMaxClipsSaved = Controls::Label(hWnd, LBL_MAX_CLIPS_SAVED, hInstance);
	LabelMaxClipsSaved.SuspendLayout();
	LabelMaxClipsSaved.SetX(10);
	LabelMaxClipsSaved.SetY(40);
	LabelMaxClipsSaved.SetText(L"Maximum clips to save:");
	LabelMaxClipsSaved.ResumeLayout();
	
	SpinnerMaxClipsSaved = Controls::Spinner(hWnd, TXT_MAX_CLIPS_SAVED, UD_MAX_CLIPS_SAVED, hInstance);
	SpinnerMaxClipsSaved.SuspendLayout();
	SpinnerMaxClipsSaved.SetX(180);
	SpinnerMaxClipsSaved.SetY(40);
	SpinnerMaxClipsSaved.SetMinValue(UserSettings::MAX_SAVED_LOWER);
	SpinnerMaxClipsSaved.SetMaxValue(UserSettings::MAX_SAVED_UPPER);
	SpinnerMaxClipsSaved.ResumeLayout();

	LabelMenuDisplayChars = Controls::Label(hWnd, LBL_MENU_DISP_CHARS, hInstance);
	LabelMenuDisplayChars.SuspendLayout();
	LabelMenuDisplayChars.SetX(10);
	LabelMenuDisplayChars.SetY(70);
	LabelMenuDisplayChars.SetText(L"Number of preview characters:");
	LabelMenuDisplayChars.ResumeLayout();
	
	SpinnerMenuDisplayChars = Controls::Spinner(hWnd, TXT_MENU_DISP_CHARS, UD_MENU_DISP_CHARS, hInstance);
	SpinnerMenuDisplayChars.SuspendLayout();
	SpinnerMenuDisplayChars.SetX(180);
	SpinnerMenuDisplayChars.SetY(70);
	SpinnerMenuDisplayChars.SetMinValue(UserSettings::MENU_CHARS_LOWER);
	SpinnerMenuDisplayChars.SetMaxValue(UserSettings::MENU_CHARS_UPPER);
	SpinnerMenuDisplayChars.ResumeLayout();
	
	LabelShowClipsHotkey = Controls::Label(hWnd, LBL_SHOW_CLIPS_HOTK, hInstance);
	LabelShowClipsHotkey.SuspendLayout();
	LabelShowClipsHotkey.SetX(10);
	LabelShowClipsHotkey.SetY(100);
	LabelShowClipsHotkey.SetText(L"Clips menu shortcut:");
	LabelShowClipsHotkey.ResumeLayout();
	HWND hHotKey = AddHotkeyCtrl(hWnd, hFontStd, 130, 100, 100, 20, hInstance, HKY_SHOW_CLIPS_MENU);

	CheckboxSaveToDisk = Controls::Checkbox(hWnd, CHK_SAVE_TO_DISK, hInstance);
	CheckboxSaveToDisk.SuspendLayout();
	CheckboxSaveToDisk.SetX(10);
	CheckboxSaveToDisk.SetY(130);
	CheckboxSaveToDisk.SetText(L"Save clips to disk");
	CheckboxSaveToDisk.UseBasicTheme = !useLightTheme;
	CheckboxSaveToDisk.ResumeLayout();

	CheckboxRunAtStartup = Controls::Checkbox(hWnd, CHK_RUN_AT_STARTUP, hInstance);
	CheckboxRunAtStartup.SuspendLayout();
	CheckboxRunAtStartup.SetX(10);
	CheckboxRunAtStartup.SetY(154);
	CheckboxRunAtStartup.SetText(L"Run Winclipper at startup");
	CheckboxRunAtStartup.UseBasicTheme = !useLightTheme;
	CheckboxRunAtStartup.ResumeLayout();

	CheckboxSelectSecondClip = Controls::Checkbox(hWnd, CHK_SLCT_2ND_CLIP, hInstance);
	CheckboxSelectSecondClip.SuspendLayout();
	CheckboxSelectSecondClip.SetX(10);
	CheckboxSelectSecondClip.SetY(178);
	CheckboxSelectSecondClip.SetText(L"Auto-select second clip");
	CheckboxSelectSecondClip.UseBasicTheme = !useLightTheme;
	CheckboxSelectSecondClip.ResumeLayout();

	CheckboxShowPreview = Controls::Checkbox(hWnd, CHK_SHOW_PREVIEW, hInstance);
	CheckboxShowPreview.SuspendLayout();
	CheckboxShowPreview.SetX(10);
	CheckboxShowPreview.SetY(202);
	CheckboxShowPreview.SetText(L"Show clip preview");
	CheckboxShowPreview.UseBasicTheme = !useLightTheme;
	CheckboxShowPreview.ResumeLayout();

	CheckboxSaveImages = Controls::Checkbox(hWnd, CHK_SAVE_IMAGES, hInstance);
	CheckboxSaveImages.SuspendLayout();
	CheckboxSaveImages.SetX(10);
	CheckboxSaveImages.SetY(226);
	CheckboxSaveImages.SetText(L"Save images to the clipboard");
	CheckboxSaveImages.UseBasicTheme = !useLightTheme;
	CheckboxSaveImages.ResumeLayout();

	LabelMaxCacheMbytes = Controls::Label(hWnd, LBL_MAX_IMG_CACHE_MB, hInstance);
	LabelMaxCacheMbytes.SuspendLayout();
	LabelMaxCacheMbytes.SetX(28);
	LabelMaxCacheMbytes.SetY(250);
	LabelMaxCacheMbytes.SetText(L"Max image cache size (MB):");
	LabelMaxCacheMbytes.ResumeLayout();
	
	SpinnerMaxCacheMbytes = Controls::Spinner(hWnd, TXT_MAX_IMG_CACHE_MB, UD_MAX_IMG_CACHE_MB, hInstance);
	SpinnerMaxCacheMbytes.SuspendLayout();
	SpinnerMaxCacheMbytes.SetX(180);
	SpinnerMaxCacheMbytes.SetY(250);
	SpinnerMaxCacheMbytes.SetMinValue(UserSettings::MAX_CACHE_MBYTES_LOWER);
	SpinnerMaxCacheMbytes.SetMaxValue(UserSettings::MAX_CACHE_MBYTES_UPPER);
	SpinnerMaxCacheMbytes.ResumeLayout();

	// Load values from user settings
	SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, UserSettings::MaxDisplayClips(), false);
	SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, UserSettings::MaxSavedClips(), false);
	SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, UserSettings::MenuDisplayChars(), false);
	SetDlgItemInt(hWnd, TXT_MAX_IMG_CACHE_MB, UserSettings::MaxCacheMegabytes(), false);

	HKEY hOpened;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened) == ERROR_SUCCESS)
	{
		if (RegistryUtilities::QueryKeyForValue(hOpened, L"Winclipper") == true)
		{
			CheckboxRunAtStartup.SetValue(true);
			//CheckDlgButton(hWnd, CHK_RUN_AT_STARTUP, BST_CHECKED);
		}
	}
	RegCloseKey(hOpened);
	CheckboxSaveToDisk.SetValue(UserSettings::SaveToDisk());
	CheckboxSelectSecondClip.SetValue(UserSettings::Select2ndClip());
	CheckboxShowPreview.SetValue(UserSettings::ShowPreview());
	CheckboxSaveImages.SetValue(UserSettings::SaveImages());
	SpinnerMaxCacheMbytes.SetEnabled(UserSettings::SaveImages());

	SendMessageW(hHotKey, HKM_SETHOTKEY, UserSettings::ClipsMenuHotkey(), 0);

	// Hide the window and update the layout
	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	return true;
}

HWND SettingsWindow::GetHandle() noexcept
{
	return windowHandle;
}

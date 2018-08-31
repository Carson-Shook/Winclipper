#include "stdafx.h"
#include "SettingsWindow.h"


ATOM SettingsWindow::RegisterSettingsWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = SettingsWindow::SettingsWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINCSETTINGS);
	wcex.lpszClassName = szSettingsWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));

	return RegisterClassExW(&wcex);
}

LRESULT SettingsWindow::SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// We want to capture a pointer to the current instance of the class
	// so we can call non-static methods from a static context.
	SettingsWindow * pThis;

	if (message == WM_NCCREATE)
	{
		pThis = static_cast<SettingsWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if (!SetWindowLongPtr(hWnd, AGNOSTIC_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<SettingsWindow*>(GetWindowLongPtr(hWnd, AGNOSTIC_USERDATA));
	}

	if (!pThis)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			pThis->WmCommandIdmAbout(hWnd, wParam, lParam);
			break;
		case IDM_EXIT:
			pThis->WmCommandIdmExit();
			break;
		case TXT_MAX_CLIPS_DISPLAY:
			pThis->WmCommandTxtMaxClipsDisplay(hWnd, wParam, lParam);
			break;
		case TXT_MAX_CLIPS_SAVED:
			pThis->WmCommandTxtMaxClipsSaved(hWnd, wParam, lParam);
			break;
		case TXT_MENU_DISP_CHARS:
			pThis->WmCommandTxtMenuDispChars(hWnd, wParam, lParam);
			break;
		case CHK_RUN_AT_STARTUP:
			pThis->WmCommandChkRunAtStartup(hWnd, wParam, lParam);
			break;
		case CHK_SAVE_TO_DISK:
			pThis->WmCommandChkSaveToDisk(hWnd, wParam, lParam);
			break;
		case CHK_SLCT_2ND_CLIP:
			pThis->WmCommandChkSlct2ndClip(hWnd, wParam, lParam);
			break;
		case CHK_SHOW_PREVIEW:
			pThis->WmCommandChkShowPreview(hWnd, wParam, lParam);
			break;
		case HKY_SHOW_CLIPS_MENU:
			pThis->WmCommandHkyShowClipsMenu(hWnd, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_LBUTTONDOWN:
		pThis->WmLbuttondown(hWnd);
		break;
	case WM_CLOSE:
		pThis->WmClose(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandIdmAbout(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SendNotifcation(IDM_ABOUT, GetHandle());
	return 0;
}

LRESULT SettingsWindow::WmCommandIdmExit()
{
	SendNotifcation(IDM_EXIT, GetHandle());
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMaxClipsDisplay(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, false);

		if (value < MAX_DISPLAY_LOWER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, MAX_DISPLAY_LOWER, false);
		}
		else if (value > MAX_DISPLAY_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, MAX_DISPLAY_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, false);

		if (value >= MAX_DISPLAY_LOWER && value <= MAX_DISPLAY_UPPER)
		{
			uSettings.SetMaxDisplayClips(value);
			if (value > uSettings.MaxSavedClips())
			{
				SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, value, false);
			}
			SendDlgItemMessage(hWnd, UD_MAX_CLIPS_SAVED, UDM_SETRANGE, 0, MAKELPARAM(MAX_SAVED_UPPER, value));
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMaxClipsSaved(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, false);

		if (value < uSettings.MaxDisplayClips())
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxDisplayClips(), false);
		}
		else if (value > MAX_SAVED_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, MAX_SAVED_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, false);

		if (value <= MAX_SAVED_UPPER && value >= uSettings.MaxDisplayClips())
		{
			uSettings.SetMaxSavedClips(GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, false));
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandTxtMenuDispChars(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, NULL, false);

		if (value < MENU_CHARS_LOWER)
		{
			SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, MENU_CHARS_LOWER, false);
		}
		else if (value > MENU_CHARS_UPPER)
		{
			SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, MENU_CHARS_UPPER, false);
		}

		return 0; // set focus on the main window again
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, NULL, false);

		if (value >= MENU_CHARS_LOWER && value <= MENU_CHARS_UPPER)
		{
			uSettings.SetMenuDisplayChars(value);
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkRunAtStartup(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (IsDlgButtonChecked(hWnd, CHK_RUN_AT_STARTUP))
		{
			DeleteRegistryRun();
			CheckDlgButton(hWnd, CHK_RUN_AT_STARTUP, BST_UNCHECKED);
		}
		else
		{
			WriteRegistryRun();
			CheckDlgButton(hWnd, CHK_RUN_AT_STARTUP, BST_CHECKED);
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkSaveToDisk(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (IsDlgButtonChecked(hWnd, CHK_SAVE_TO_DISK))
		{
			uSettings.SetSaveToDisk(false);
			CheckDlgButton(hWnd, CHK_SAVE_TO_DISK, BST_UNCHECKED);
		}
		else
		{
			uSettings.SetSaveToDisk(true);
			CheckDlgButton(hWnd, CHK_SAVE_TO_DISK, BST_CHECKED);
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkSlct2ndClip(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (IsDlgButtonChecked(hWnd, CHK_SLCT_2ND_CLIP))
		{
			uSettings.SetSelect2ndClip(false);
			CheckDlgButton(hWnd, CHK_SLCT_2ND_CLIP, BST_UNCHECKED);
		}
		else
		{
			uSettings.SetSelect2ndClip(true);
			CheckDlgButton(hWnd, CHK_SLCT_2ND_CLIP, BST_CHECKED);
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmCommandChkShowPreview(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (IsDlgButtonChecked(hWnd, CHK_SHOW_PREVIEW))
		{
			uSettings.SetShowPreview(false);
			CheckDlgButton(hWnd, CHK_SHOW_PREVIEW, BST_UNCHECKED);
		}
		else
		{
			uSettings.SetShowPreview(true);
			CheckDlgButton(hWnd, CHK_SHOW_PREVIEW, BST_CHECKED);
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
		wHotkey = (WORD)SendMessage(GetDlgItem(hWnd, HKY_SHOW_CLIPS_MENU), HKM_GETHOTKEY, 0, 0);

		// Resets to the default hotkey if the combo is cleared.
		if (wHotkey == 0)
		{
			SendMessage(GetDlgItem(hWnd, HKY_SHOW_CLIPS_MENU), HKM_SETHOTKEY, CMENU_HOTKEY_DEF, 0);
			uSettings.SetClipsMenuHotkey(CMENU_HOTKEY_DEF);
		}
		else
		{
			// So the WORD returned from HKM_GETHOTKEY stuffs all of the
			// hotkey info in the LOWORD side into the LOBYTE and HIBYTE
			// intead of on each side of the WORD, so we have to split
			// these out for consistency with storing in settings.
			uSettings.SetClipsMenuHotkey(MAKEWORD(LOBYTE(LOWORD(wHotkey)), HIBYTE(LOWORD(wHotkey))));
		}
	}
	return 0;
}

LRESULT SettingsWindow::WmLbuttondown(HWND hWnd)
{
	SetFocus(hWnd);
	return 0;
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

	GetModuleFileName(0, pPath, MAX_PATH);

	//OpenRegistryRun(hOpened);
	RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened);

	if (RegSetValueEx(hOpened, L"Winclipper", 0, REG_SZ, (LPBYTE)pPath, sizeof(pPath)) != ERROR_SUCCESS)
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
		RegDeleteValue(hOpened, L"Winclipper");
	}
	else
	{
		return false;
	}

	RegCloseKey(hOpened);
	return true;
}

SettingsWindow::SettingsWindow(HINSTANCE hInstance, UserSettings & userSettings)
	:uSettings(userSettings)
{
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINCSETTINGS, szSettingsWindowClass, MAX_LOADSTRING);

	RegisterSettingsWindowClass(hInstance);
}

SettingsWindow::~SettingsWindow()
{
}

bool SettingsWindow::InitSettingsWindow(HINSTANCE hInstance, HWND parentWnd)
{

	HWND hWnd = CreateWindowEx(0, szSettingsWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, ScaleX(260), ScaleY(284), parentWnd, nullptr, hInstance, this);

	if (!hWnd)
	{
		return false;
	}

	// Still trying to figure out how to not do this.
	windowHandle = hWnd;

	NONCLIENTMETRICS hfDefault;
	hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
	hFontStd = CreateFontIndirectW(&hfDefault.lfCaptionFont);

	// Add controls in tab order
	AddLabel(hWnd, hFontStd, 10, 10, hInstance, L"Number of clips to display:", LBL_MAX_CLIPS_DISPLAY);
	AddSpinner(hWnd, hFontStd, 180, 10, hInstance, MAX_DISPLAY_LOWER, MAX_DISPLAY_UPPER, UD_MAX_CLIPS_DISPLAY, TXT_MAX_CLIPS_DISPLAY);
	AddLabel(hWnd, hFontStd, 10, 40, hInstance, L"Maximum clips to save:", LBL_MAX_CLIPS_SAVED);
	AddSpinner(hWnd, hFontStd, 180, 40, hInstance, MAX_SAVED_LOWER, MAX_SAVED_UPPER, UD_MAX_CLIPS_SAVED, TXT_MAX_CLIPS_SAVED);

	AddLabel(hWnd, hFontStd, 10, 70, hInstance, L"Number of preview characters:", LBL_MENU_DISP_CHARS);
	AddSpinner(hWnd, hFontStd, 180, 70, hInstance, MENU_CHARS_LOWER, MENU_CHARS_UPPER, UD_MENU_DISP_CHARS, TXT_MENU_DISP_CHARS);

	AddLabel(hWnd, hFontStd, 10, 100, hInstance, L"Clips menu shortcut:", LBL_SHOW_CLIPS_HOTK);
	HWND hHotKey = AddHotkeyCtrl(hWnd, hFontStd, 130, 100, 100, 20, hInstance, HKY_SHOW_CLIPS_MENU);

	AddCheckbox(hWnd, hFontStd, 10, 130, hInstance, L"Save clips to disk", CHK_SAVE_TO_DISK);
	AddCheckbox(hWnd, hFontStd, 10, 154, hInstance, L"Run Winclipper at startup", CHK_RUN_AT_STARTUP);
	AddCheckbox(hWnd, hFontStd, 10, 178, hInstance, L"Auto-select second clip", CHK_SLCT_2ND_CLIP);
	AddCheckbox(hWnd, hFontStd, 10, 202, hInstance, L"Show clip preview", CHK_SHOW_PREVIEW);

	// Load values from user settings
	SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, uSettings.MaxDisplayClips(), false);
	SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxSavedClips(), false);

	SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, uSettings.MenuDisplayChars(), false);

	HKEY hOpened;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened) == ERROR_SUCCESS)
	{
		if (RegistryUtilities::QueryKeyForValue(hOpened, L"Winclipper") == true)
		{
			CheckDlgButton(hWnd, CHK_RUN_AT_STARTUP, BST_CHECKED);
		}
	}
	RegCloseKey(hOpened);
	if (uSettings.SaveToDisk())
	{
		CheckDlgButton(hWnd, CHK_SAVE_TO_DISK, BST_CHECKED);
	}
	if (uSettings.Select2ndClip())
	{
		CheckDlgButton(hWnd, CHK_SLCT_2ND_CLIP, BST_CHECKED);
	}
	if (uSettings.ShowPreview())
	{
		CheckDlgButton(hWnd, CHK_SHOW_PREVIEW, BST_CHECKED);
	}

	SendMessage(hHotKey, HKM_SETHOTKEY, uSettings.ClipsMenuHotkey(), 0);

	// Hide the window and update the layout
	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	return true;
}

HWND SettingsWindow::GetHandle()
{
	return windowHandle;
}

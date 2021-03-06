#include "stdafx.h"
#include "MainWindow.h"


ATOM MainWindow::RegisterMainWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWindow::MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szMainWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));

	return RegisterClassExW(&wcex);
}

LRESULT MainWindow::MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// We want to capture a pointer to the current instance of the class
	// so we can call non-static methods from a static context.
	MainWindow * pThis = nullptr;

	if (message == WM_NCCREATE)
	{
#pragma warning( suppress : 26490 )
		pThis = static_cast<MainWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
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
		pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hWnd, AGNOSTIC_USERDATA));
	}

	if (!pThis)
	{
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_CLEARCLIPS:
			pThis->WmCommandIdmClearClips(hWnd, wParam, lParam);
			break;
		case IDM_RECREATE_THUMBNAILS:
			pThis->WmCommandIdmRecreateThumbnails(hWnd, wParam, lParam);
			break;
		case IDM_SETTINGS:
			pThis->WmCommandIdmSettings(hWnd, wParam, lParam);
			break;
		case IDM_ABOUT:
			pThis->WmCommandIdmAbout(hWnd, wParam, lParam);
			break;
		case IDM_EXIT:
			pThis->WmCommandIdmExit(hWnd, wParam, lParam);
			break;
		case NTF_CMENUHOTKEY_CHANGED:
			pThis->WmCommandNtfCmenuHotkeyChanged(hWnd, wParam, lParam);
			break;
		case NTF_MAXDISPLAY_CHANGED:
			pThis->WmCommandNtfMaxDisplayChanged(hWnd, wParam, lParam);
			break;
		case NTF_MAXSAVED_CHANGED:
			pThis->WmCommandNtfMaxSavedChanged(hWnd, wParam, lParam);
			break;
		case NTF_MENUCHARS_CHANGED:
			pThis->WmCommandNtfMenuCharsChanged(hWnd, wParam, lParam);
			break;
		case NTF_SAVETODISK_CHANGED:
			pThis->WmCommandNtfSaveToDiskChanged(hWnd, wParam, lParam);
			break;
		case NTF_SAVEIMAGES_CHANGED:
			pThis->WmCommandNtfSaveImagesChanged(hWnd, wParam, lParam);
			break;
		case NTF_MAXCACHEMBYTES_CHANGED:
			pThis->WmCommandNtfMaxcacheMBytesChanged(hWnd, wParam, lParam);
			break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WMAPP_NOTIFYCALLBACK:
		// React to clicks on the notify icon
		switch (LOWORD(lParam))
		{
		case NIN_KEYSELECT:
			pThis->WmappNCallNinKeySelect(hWnd, wParam, lParam);
			break;
		case WM_LBUTTONDBLCLK:
			pThis->WmappNCallWmLButtonDblClk(hWnd, wParam, lParam);
			break;
		case WM_CONTEXTMENU:
			pThis->WmappNCallWmContextMenu(hWnd, wParam, lParam);
			break;
		}
		break;
	case WM_CLIPBOARDUPDATE:
		pThis->WmClipboardUpdate(hWnd, wParam, lParam);
		break;
	case WM_HOTKEY:
		pThis->WmHotkey(hWnd, wParam, lParam);
		break;
	case WM_CREATE:
		pThis->WmCreate(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		pThis->WmDestroy(hWnd, wParam, lParam);
		break;
	case WM_INITMENU:
		pThis->WmInitMenu(hWnd, wParam, lParam);
		break;
	case WM_INITMENUPOPUP:
		pThis->WmInitMenuPopup(hWnd, wParam, lParam);
		break;
	case WM_MENURBUTTONUP:
		pThis->WmMenuRButtonUp(hWnd, wParam, lParam);
		break;
	case WM_UNINITMENUPOPUP:
		pThis->WmUninitMenuPopup(hWnd, wParam, lParam);
		break;
	case WM_MENUSELECT:
		pThis->WmMenuSelect(hWnd, wParam, lParam);
		break;
	default:
		{
			if (message == taskbarRestart)
			{
				pThis->WmRestartTaskbar(hWnd, wParam, lParam);
			}
			pThis->WndProcDefault(hWnd, wParam, lParam);
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK MainWindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)true;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)true;
		}
		break;
	}
	return (INT_PTR)false;
}

LRESULT MainWindow::WmCommandIdmClearClips(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int choice = MessageBoxA(hWnd,
		"This will delete all clips on the Winclipper clipboard.\r\n\r\n Are you sure you want to do this?", "Winclipper",
		MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TASKMODAL | MB_SETFOREGROUND);

	if (choice == IDYES)
	{
		cManager->ClearClips();
	}
	return 0;
}

LRESULT MainWindow::WmCommandIdmRecreateThumbnails(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	cManager->RecreateThumbnails();
	return 0;
}

LRESULT MainWindow::WmCommandIdmSettings(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND settingsHwnd = settingsWindow.GetHandle();

	if (!IsWindowVisible(settingsHwnd))
	{
		ShowWindow(settingsHwnd, SW_SHOW);
	}
	SetForegroundWindow(settingsHwnd);

	return 0;
}

LRESULT MainWindow::WmCommandIdmAbout(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND sender = (HWND)lParam == nullptr ? hWnd : (HWND)lParam;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), sender, About);
	return 0;
}

LRESULT MainWindow::WmCommandIdmExit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// wait until finished writing, or give up after 10 seconds (should be long enough)
	int waitUntilFinishedAttempts = 5;
	while (!(cManager->NoPendingClipWrites() && UserSettings::NoPendingSettingWrites()) && waitUntilFinishedAttempts > 0)
	{
		Sleep(2000);
		waitUntilFinishedAttempts--;
	}
	OnDemandBitmapManager::SafeClose();
	DestroyWindow(hWnd);

	return 0;
}

LRESULT MainWindow::WmCommandNtfCmenuHotkeyChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UnregisterHotKey(GetHandle(), ID_REG_HOTKEY);
	RegisterHotKey(GetHandle(), ID_REG_HOTKEY, HIBYTE(UserSettings::ClipsMenuHotkeyTrl()), LOBYTE(UserSettings::ClipsMenuHotkeyTrl()));
	return 0;
}

LRESULT MainWindow::WmCommandNtfMaxDisplayChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT MainWindow::WmCommandNtfMaxSavedChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	cManager->UpdateMaxClips();
	return 0;
}

LRESULT MainWindow::WmCommandNtfMenuCharsChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT MainWindow::WmCommandNtfSaveToDiskChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	cManager->SetSaveToDisk(UserSettings::SaveToDisk());
	return 0;
}

LRESULT MainWindow::WmCommandNtfSaveImagesChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	cManager->UpdateSavedImages();
	return 0;
}

LRESULT MainWindow::WmCommandNtfMaxcacheMBytesChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	OnDemandBitmapManager::SetMaxBytes(unsigned long long{ UserSettings::MaxCacheMegabytes() } * 1048576);
	return 0;
}

LRESULT MainWindow::WmappNCallNinKeySelect(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// This prevents any additional keypresses from being 
	// interpreted until after ShowClipsMenu has finished.
	if (!ninKeypressLocked)
	{
		ninKeypressLocked = true;
		const int xCoord GET_X_LPARAM(wParam);
		const int yCoord GET_Y_LPARAM(wParam);

		POINT cPos = { 0, 0 };
		if (xCoord != NULL && yCoord != NULL)
		{
			cPos.x = xCoord;
			cPos.y = yCoord;
		}
		else
		{
			cPos.x = 10;
			cPos.y = 10;
		}
		cManager->ShowClipsMenu(GetHandle(), &cPos, true);
		previewWindow->ClearCache();
		ninKeypressLocked = false;
	}
	return 0;
}

LRESULT MainWindow::WmappNCallWmLButtonDblClk(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND settingsHwnd = settingsWindow.GetHandle();

	if (!IsWindowVisible(settingsHwnd))
	{
		ShowWindow(settingsHwnd, SW_SHOW);
	}
	SetForegroundWindow(settingsHwnd);

	return 0;
}

LRESULT MainWindow::WmappNCallWmContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT cPos = { 0, 0 };
	if (!GetCursorPos(&cPos))
	{
		cPos.x = 10;
		cPos.y = 10;
	}
	cManager->ShowClipsMenu(GetHandle(), &cPos, true);
	previewWindow->ClearCache();

	return 0;
}

LRESULT MainWindow::WmClipboardUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!cManager->ClipLock)
	{
		// Add clips to the clips manager when the clipboard updates.
		bool success = false;
		int retries = 10;
		while (!success && retries > 0)
		{
			success = cManager->AddToClips(hWnd);
			Sleep(100);
			retries--;
		}
	}
	else
	{
		cManager->ClipLock = false;
	}
	return 0;
}

LRESULT MainWindow::WmHotkey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// When the global hotkey is called, get the current window
	// so we can reactivate it later, and then show the clips menu.
	POINT cPos = { 0, 0 };
	if (!GetCursorPos(&cPos))
	{
		cPos.x = 10;
		cPos.y = 10;
	}
	cManager->ShowClipsMenu(GetHandle(), &cPos, false);
	previewWindow->ClearCache();

	return 0;
}

LRESULT MainWindow::WmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	taskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
	// Add the notification icon
	if (!AddNotificationIcon(hWnd, ((LPCREATESTRUCT) lParam)->hInstance))
	{
		MessageBox(hWnd,
			L"There was an error adding the notification icon. You might want to try restarting your computer, or reinstalling this application.",
			L"Error adding icon", MB_OK);

		return -1;
	}

	UserSettings::Subscribe(hWnd);
	SettingsWindow::Subscribe(hWnd);

	cManager = new ClipsManager();
	return 0;
}

LRESULT MainWindow::WmDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	delete previewWindow;
	delete cManager;
	previewWindow = nullptr;
	cManager = nullptr;
	DeleteNotificationIcon();
	PostQuitMessage(0);
	return 0;
}

LRESULT MainWindow::WmInitMenu(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// keep track of the top level menu to differentiate for display purposes
	activePopupMenu = topPopupMenu = (HMENU)wParam;
	cManager->SelectDefaultMenuItem(cManager->GetSize() > 1 ? UserSettings::Select2ndClip() : false);
	return 0;
}

LRESULT MainWindow::WmMenuRButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HMENU menu = CreatePopupMenu();
	AppendMenuW(menu, MF_STRING, 1, L"Delete");

	POINT cPos = { 0, 0 };
	if (!GetCursorPos(&cPos))
	{
		cPos.x = 10;
		cPos.y = 10;
	}

	int menuSelection;
	menuSelection = TrackPopupMenuEx(
		menu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_RECURSE,
		cPos.x - 5,
		cPos.y - 5,
		hWnd,
		nullptr
	);

	if (menuSelection == 1)
	{
		unsigned int index = LOWORD(wParam);

		if (activePopupMenu != topPopupMenu)
		{
			index += UserSettings::MaxDisplayClips();
		}
		cManager->DeleteClipAt(index);

		PostMessageW(hWnd, WM_CANCELMODE, 0, 0);
	}

	return 0;
}

LRESULT MainWindow::WmInitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// whenever a submenu appears, we save it the active popup
	activePopupMenu = (HMENU)wParam;
	return 0;
}

LRESULT MainWindow::WmUninitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// when a submenu is destroyed, we set the active popup back to the top
	activePopupMenu = topPopupMenu;
	return 0;
}

LRESULT MainWindow::WmMenuSelect(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (UserSettings::ShowPreview())
	{
		// Display preview window when user hovers on a clip
		DWORD flags = HIWORD(wParam);
		if ((flags & MF_HILITE) && !(flags & MF_POPUP))
		{
			const unsigned int i = LOWORD(wParam);
			if (i != 0 && i <= cManager->GetSize())
			{
				previewWindow->SetPreviewClip(cManager->GetClipAt(i - 1));

				unsigned int offset = 1;

				if (activePopupMenu != topPopupMenu)
				{
					offset += UserSettings::MaxDisplayClips();
				}

				/* BLACK MAGIC 0_0 */
				// This gets a MenuItemRect for an item position we know (0),
				// and then performs a hit-test on the menuItem that sits over.
				// This might be the same item, or it could be a different one
				// if the menu has been scrolled. That is our delta value that
				// can be used to calculate the true offset for the preview.
				// We do this by getting the real rect at the top of the menu
				// and subtracting it's bottom value from the original first
				// item's rect top, and then we can use the resulting value
				// as an offset in pixels to subtract from our desired item.
				RECT hitTestRect = { 0, 0, 0, 0 };
				int hitTestDelta = 0;
				long sizeOffset = 0;
				if (GetMenuItemRect(hWnd, activePopupMenu, 0, &hitTestRect))
				{
					POINT hitTest = { hitTestRect.left + 1, hitTestRect.top + 1 };
					hitTestDelta = MenuItemFromPoint(hWnd, activePopupMenu, hitTest);

					RECT currentRect = { 0, 0, 0, 0 };
					if (GetMenuItemRect(hWnd, activePopupMenu, hitTestDelta - 1, &currentRect))
					{
						sizeOffset = currentRect.bottom - hitTestRect.top;
					}
				}

				RECT menuItemDims = { 0, 0, 0, 0 };
				if (GetMenuItemRect(hWnd, activePopupMenu, i - offset, &menuItemDims))
				{
					menuItemDims.top = menuItemDims.top - sizeOffset;
					menuItemDims.bottom = menuItemDims.bottom - sizeOffset;
					previewWindow->MoveRelativeToRect(&menuItemDims, i);
					previewWindow->Show();
				}
			}
			else
			{
				previewWindow->Hide();
			}
		}
		else
		{
			previewWindow->Hide();
		}
	}
	return 0;
}
LRESULT MainWindow::WmRestartTaskbar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!AddNotificationIcon(hWnd, hInst))
	{
		MessageBox(hWnd,
			L"There was an error adding the notification icon. You might want to try restarting your computer, or reinstalling this application.",
			L"Error adding icon", MB_OK);

		return -1;
	}
	return 0;
}

// This ensures that if the mouse is moved outside of the popup menu,
// then the preview window will be hidden. This is achieved by 
// comparing the previous mouse location against the current, and if
// they are different, we check to see if they are now selecting a
// menuItem. If not, then hide the preview window.
LRESULT MainWindow::WndProcDefault(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (previewWindow != nullptr && IsWindowVisible(previewWindow->GetHandle()))
	{
		POINT cPos = { 0, 0 };
		if (GetCursorPos(&cPos))
		{
			if (cPos.x != previousMouseLoc->x || cPos.y != previousMouseLoc->y)
			{
				if (MenuItemFromPoint(hWnd, activePopupMenu, cPos) == -1)
				{
					previewWindow->Hide();
				}

				previousMouseLoc->x = cPos.x;
				previousMouseLoc->y = cPos.y;

			}
		}
	}
	return 0;
}

// Adds the application notification icon to the notifcation area
bool MainWindow::AddNotificationIcon(HWND hWnd, HINSTANCE hInstance)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
	nid.uID =
		// Load the icon for high DPI.
		LoadIconMetric(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER), LIM_SMALL, &nid.hIcon);
	// Show the notification.
	Shell_NotifyIcon(NIM_ADD, &nid);

	nid.uVersion = NOTIFYICON_VERSION_4;
	return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

// Removes the application notification icon from the notifcation area
bool MainWindow::DeleteNotificationIcon()
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.uID = UID_NOTIFYICON;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

MainWindow::MainWindow(HINSTANCE hInstance, SettingsWindow & settingsWindow)
	:settingsWindow(settingsWindow)
{
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINCLIPPER, szMainWindowClass, MAX_LOADSTRING);

	RegisterMainWindowClass(hInstance);
	if (InitMainWindow(hInstance))
	{
		previewWindow = new PreviewWindow(hInstance, GetHandle());
	}

	// used for the dialog box.
	hInst = hInstance;
}

MainWindow::~MainWindow()
{
}

bool MainWindow::InitMainWindow(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindowExW(0, szMainWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		0, 0, 100, 100, nullptr, nullptr, hInstance, this);

	if (!hWnd)
	{
		return false;
	}

	windowHandle = hWnd;

	// Adds the main window to the clipboard format listener list.
	AddClipboardFormatListener(hWnd);

	// Register the hotkey that we have stored in settings
	RegisterHotKey(hWnd, ID_REG_HOTKEY, HIBYTE(UserSettings::ClipsMenuHotkeyTrl()), LOBYTE(UserSettings::ClipsMenuHotkeyTrl()));

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	OnDemandBitmapManager::SetMaxBytes(unsigned long long{ UserSettings::MaxCacheMegabytes() } * 1048576);

	return true;
}

HWND MainWindow::GetHandle()
{
	return windowHandle;
}

UINT MainWindow::taskbarRestart = 0;


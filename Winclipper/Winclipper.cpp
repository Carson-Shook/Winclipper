// Winclipper.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "resource.h"
#include "string.h"
#include "UserSettings.h"
#include "ControlUtilities.h"
#include "ClipsMenu.h"
#include "Winclipper.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCLIPPER, szMainWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCSETTINGS, szSettingsWindowClass, MAX_LOADSTRING);
    RegisterMainClass(hInstance);
    RegisterSettingsWindowClass(hInstance);

    hInst = hInstance; // Store instance handle in our global variable

    if (!InitMainWindow (hInstance, nCmdShow))
    {
        return FALSE;
    }
    if (!InitSettingsWindow(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINCSETTINGS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable,  &msg))
        {
            if (!IsDialogMessage(GetAncestor(msg.hwnd,GA_ROOTOWNER), &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return (int) msg.wParam;
}

ATOM RegisterMainClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szMainWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));

    return RegisterClassExW(&wcex);
}

ATOM RegisterSettingsWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = SettingsWndProc;
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

// Initializes the main window. The main window is where
// the clips menu is called from.
BOOL InitMainWindow(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindowExW(0, szMainWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Still trying to figure out how to not do this. I wish events were a thing.
   mainWnd = hWnd;

   // Adds the main window to the clipboard format listener list.
   AddClipboardFormatListener(hWnd);

   // Register the hotkey that we have stored in settings
   RegisterHotKey(hWnd, ID_REG_HOTKEY, HIBYTE(uSettings.ClipsMenuHotkeyTrl()), LOBYTE(uSettings.ClipsMenuHotkeyTrl()));

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   
   return TRUE;
}

// Initializes the settings window. This window contains
// all of the available user settings. For the most part,
// we don't care about the handles of each control. Just make 
// sure that they all have an ID so we can reference them later.
BOOL InitSettingsWindow(HINSTANCE hInstance, int nCmdShow)
{

    HWND hWnd = CreateWindowExW(0, szSettingsWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, ScaleX(260), ScaleY(240), nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Still trying to figure out how to not do this.
    settingsWnd = hWnd;

    NONCLIENTMETRICS hfDefault;
    hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
    HFONT font = CreateFontIndirectW(&hfDefault.lfCaptionFont);

    // Add controls in tab order
    AddLabel(hWnd, font, 10, 10, hInstance, _T("Number of clips to display:"), LBL_MAX_CLIPS_DISPLAY);
    AddSpinner(hWnd, font, 180, 10, hInstance, MAX_DISPLAY_LOWER, MAX_DISPLAY_UPPER, UD_MAX_CLIPS_DISPLAY, TXT_MAX_CLIPS_DISPLAY);
    AddLabel(hWnd, font, 10, 40, hInstance, _T("Maximum clips to save:"), LBL_MAX_CLIPS_SAVED);
    AddSpinner(hWnd, font, 180, 40, hInstance, MAX_SAVED_LOWER, MAX_SAVED_UPPER, UD_MAX_CLIPS_SAVED, TXT_MAX_CLIPS_SAVED);
    
    AddLabel(hWnd, font, 10, 70, hInstance, _T("Number of preview characters:"), LBL_MENU_DISP_CHARS);
    AddSpinner(hWnd, font, 180, 70, hInstance, MENU_CHARS_LOWER, MENU_CHARS_UPPER, UD_MENU_DISP_CHARS, TXT_MENU_DISP_CHARS);

    AddLabel(hWnd, font, 10, 100, hInstance, _T("Clips menu shortcut:"), LBL_SHOW_CLIPS_HOTK);
    HWND hHotKey = AddHotkeyCtrl(hWnd, font, 130, 100, 100, 20, hInstance, HKY_SHOW_CLIPS_MENU);

    AddCheckbox(hWnd, font, 10, 130, hInstance, _T("Save clips to disk"), CHK_SAVE_TO_DISK);
    AddCheckbox(hWnd, font, 10, 154, hInstance, _T("Run Winclipper at startup"), CHK_RUN_AT_STARTUP);



    // Load values from user settings
    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, uSettings.MaxDisplayClips(), FALSE);
    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxSavedClips(), FALSE);

    SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, uSettings.MenuDisplayChars(), FALSE);

    HKEY hOpened;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hOpened) == ERROR_SUCCESS)
    {
        if (QueryKeyForValue(hOpened, _T("Winclipper")) == TRUE)
        {
            CheckDlgButton(hWnd, CHK_RUN_AT_STARTUP, BST_CHECKED);
        }
    }
    RegCloseKey(hOpened);
    if (uSettings.SaveToDisk())
    {
        CheckDlgButton(hWnd, CHK_SAVE_TO_DISK, BST_CHECKED);
    }

    SendMessage(hHotKey, HKM_SETHOTKEY, uSettings.ClipsMenuHotkey(), 0);

    // Hide the window and update the layout
    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    return TRUE;
}

// Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_CLEARCLIPS:
            cManager.ClearClips();
            break;
        case IDM_SETTINGS:
            if (!IsWindowVisible(settingsWnd))
            {
                ShowWindow(settingsWnd, SW_SHOW);
            }
            SetForegroundWindow(settingsWnd);
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WMAPP_NOTIFYCALLBACK:
        // React to clicks on the notify icon
        switch (LOWORD(lParam))
        {
        case WM_LBUTTONDBLCLK:
        {
            if (!IsWindowVisible(hWnd))
            {
                ShowWindow(settingsWnd, SW_SHOW);
            }
            SetForegroundWindow(settingsWnd);
        }
            break;
        case WM_CONTEXTMENU:
        {
            HWND curWin = GetForegroundWindow();
            ShowClipsMenu(hWnd, curWin, cManager, TRUE);
        }
        break;
        }
    break;
    case WM_CLIPBOARDUPDATE:
    {
        // Add clips to the clips manager when the clipboard updates.
        cManager.AddToClips(hWnd);
    }
    break;
    case WM_HOTKEY:
    {
        // When the global hotkey is called, get the current window
        // so we can reactivate it later, and then show the clips menu.
        HWND curWin = GetForegroundWindow();
        ShowClipsMenu(hWnd, curWin, cManager, FALSE);
    }
    break;
    case WM_CREATE:
    {
        // Add the notification icon
        if (!AddNotificationIcon(hWnd))
        {
            MessageBox(hWnd,
                _T("There was an error adding the notification icon. You might want to try restarting your computer, or reinstalling this application."),
                _T("Error adding icon"), MB_OK);
            
            return -1;
        }
    }
    break;
    case WM_DESTROY:
    {
        DeleteNotificationIcon();
        PostQuitMessage(0);
    }
        break;
    case WM_MENUSELECT:
    {
        // Will use this case later for hover-previews when they're implemented
        DWORD flags = HIWORD(wParam);
        if (flags && MF_HILITE != 0)
        {
            int i = LOWORD(wParam);
            int j = i;
        }
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Processes messages for the settings window.
LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_CLEARCLIPS:
            cManager.ClearClips();
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(mainWnd);
            break;
        case TXT_MAX_CLIPS_DISPLAY:
        {
            if (HIWORD(wParam) == EN_KILLFOCUS)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, FALSE);

                if (value < MAX_DISPLAY_LOWER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, MAX_DISPLAY_LOWER, FALSE);
                }
                else if (value > MAX_DISPLAY_UPPER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, MAX_DISPLAY_UPPER, FALSE);
                }

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, FALSE);

                if (value >= MAX_DISPLAY_LOWER && value <= MAX_DISPLAY_UPPER)
                {
                    cManager.SetDisplayClips(value);
                    uSettings.SetMaxDisplayClips(value);
                    if (value > uSettings.MaxSavedClips())
                    {
                        SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, value, FALSE);
                    }
                    SendDlgItemMessage(hWnd, UD_MAX_CLIPS_SAVED, UDM_SETRANGE, 0, MAKELPARAM(MAX_SAVED_UPPER, value));
                }
            }
        }
        break;
        case TXT_MAX_CLIPS_SAVED:
        {
            if (HIWORD(wParam) == EN_KILLFOCUS)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, FALSE);

                if (value < uSettings.MaxDisplayClips())
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxDisplayClips(), FALSE);
                }
                else if (value > MAX_SAVED_UPPER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, MAX_SAVED_UPPER, FALSE);
                }

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, FALSE);

                if (value <= MAX_SAVED_UPPER && value >= uSettings.MaxDisplayClips())
                {
                    cManager.SetMaxClips(value);
                    uSettings.SetMaxSavedClips(GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, FALSE));
                }
            }
        }
        break;
        case TXT_MENU_DISP_CHARS:
        {
            if (HIWORD(wParam) == EN_KILLFOCUS)
            {
                int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, NULL, FALSE);

                if (value < MENU_CHARS_LOWER)
                {
                    SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, MENU_CHARS_LOWER, FALSE);
                }
                else if (value > MENU_CHARS_UPPER)
                {
                    SetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, MENU_CHARS_UPPER, FALSE);
                }

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, NULL, FALSE);

                if (value >= MENU_CHARS_LOWER && value <= MENU_CHARS_UPPER)
                {
                    cManager.SetMenuDisplayChars(value);
                    uSettings.SetMenuDisplayChars(value);
                }
            }
        }
        break;
        case CHK_RUN_AT_STARTUP:
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
        }
        break;
        case CHK_SAVE_TO_DISK:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (IsDlgButtonChecked(hWnd, CHK_SAVE_TO_DISK))
                {
                    cManager.SetSaveToDisk(false);
                    uSettings.SetSaveToDisk(false);
                    CheckDlgButton(hWnd, CHK_SAVE_TO_DISK, BST_UNCHECKED);
                }
                else
                {
                    cManager.SetSaveToDisk(true);
                    uSettings.SetSaveToDisk(true);
                    CheckDlgButton(hWnd, CHK_SAVE_TO_DISK, BST_CHECKED);
                }
            }
        }
        break;
        case HKY_SHOW_CLIPS_MENU:
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
                UnregisterHotKey(mainWnd, ID_REG_HOTKEY);
                RegisterHotKey(mainWnd, ID_REG_HOTKEY, HIBYTE(uSettings.ClipsMenuHotkeyTrl()), LOBYTE(uSettings.ClipsMenuHotkeyTrl()));
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

    }
    break;
    case WM_LBUTTONDOWN:
    {
        SetFocus(hWnd);
    }
    break;
    case WM_CLOSE:
    {
        SetFocus(hWnd);
        ShowWindow(hWnd, SW_HIDE);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Writes the "run at startup" registry key for Winclipper
BOOL WriteRegistryRun()
{
    HKEY hOpened;
    TCHAR pPath[100];

    GetModuleFileName(0, pPath, 100);

    //OpenRegistryRun(hOpened);
    RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hOpened);

    if (RegSetValueEx(hOpened, _T("Winclipper"), 0, REG_SZ, (LPBYTE)pPath, sizeof(pPath)) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    RegCloseKey(hOpened);
    return TRUE;
}

// Deletes the "run at startup" registry key for Winclipper
BOOL DeleteRegistryRun()
{
    HKEY hOpened;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hOpened) == ERROR_SUCCESS)
    {
        RegDeleteValue(hOpened, _T("Winclipper"));
    }
    else
    {
        return FALSE;
    }

    RegCloseKey(hOpened);
    return TRUE;
}

// Get the value of a key in the registry if one exists
BOOL QueryKeyForValue(HKEY hKey, TCHAR* checkValue)
{
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys = 0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i, retCode;

    TCHAR  achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;

    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

                                 // Enumerate the key values. 

    if (cValues)
    {
        for (i = 0, retCode = ERROR_SUCCESS; i<cValues; i++)
        {
            cchValue = MAX_VALUE_NAME;
            achValue[0] = '\0';
            retCode = RegEnumValue(hKey, i,
                achValue,
                &cchValue,
                NULL,
                NULL,
                NULL,
                NULL);

            if (retCode == ERROR_SUCCESS)
            {
                if (_tcscmp(checkValue, achValue) == 0)
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}


// Adds the application notification icon to the notifcation area
BOOL AddNotificationIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.uID = 
    // Load the icon for high DPI.
    LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_WINCLIPPER), LIM_LARGE, &nid.hIcon);
    
    // Show the notification.
    Shell_NotifyIcon(NIM_ADD, &nid);

    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

// Removes the application notification icon from the notifcation area
BOOL DeleteNotificationIcon()
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.uID = UID_NOTIFYICON;
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}
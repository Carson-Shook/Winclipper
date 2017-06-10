// Winclipper.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "resource.h"
#include "string.h"
#include "SettingsManager.h"
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

BOOL InitMainWindow(HINSTANCE hInstance, int nCmdShow)
{

   HWND hWnd = CreateWindowExW(0, szMainWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   mainWnd = hWnd;

   AddClipboardFormatListener(hWnd);
   RegisterHotKey(hWnd, ID_REG_HOTKEY, MOD_SHIFT | MOD_CONTROL | MOD_NOREPEAT, 0x56);

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   
   return TRUE;
}

BOOL InitSettingsWindow(HINSTANCE hInstance, int nCmdShow)
{

    HWND hWnd = CreateWindowExW(0, szSettingsWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    settingsWnd = hWnd;

    NONCLIENTMETRICS hfDefault;
    hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
    HFONT font = CreateFontIndirectW(&hfDefault.lfCaptionFont);

    AddLabel(hWnd, font, 10, 10, hInstance, _T("Number of clips to display"), LBL_MAX_CLIPS_DISPLAY);
    AddSpinner(hWnd, font, 230, 10, hInstance, MAX_DISPLAY_LOWER, MAX_DISPLAY_UPPER, UD_MAX_CLIPS_DISPLAY, TXT_MAX_CLIPS_DISPLAY);
    AddLabel(hWnd, font, 360, 10, hInstance, _T("Maximum clips to save"), LBL_MAX_CLIPS_SAVED);
    AddSpinner(hWnd, font, 550, 10, hInstance, MAX_SAVED_LOWER, MAX_SAVED_UPPER, UD_MAX_CLIPS_SAVED, TXT_MAX_CLIPS_SAVED);

    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, uSettings.MaxDisplayClips(), TRUE);
    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxSavedClips(), TRUE);

    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
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
            //POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
            //ShowContextMenu(hWnd, pt);

            HWND curWin = GetForegroundWindow();
            ShowClipsMenu(hWnd, curWin, cManager, TRUE);
        }
        break;
        }
    break;
    case WM_CLIPBOARDUPDATE:
    {
        cManager.AddToClips(hWnd);

        if (!OpenClipboard(hWnd))
            return FALSE;
        HANDLE psClipboardData = GetClipboardData(CF_UNICODETEXT);
        if (psClipboardData != NULL)
        {
            TCHAR * data = (TCHAR*)(GlobalLock(psClipboardData));
            if (data != NULL)
            {
                TCHAR * derefData = _wcsdup(data);
            }
            GlobalUnlock(psClipboardData);
        }
        CloseClipboard();
    }
    break;
    case WM_HOTKEY:
    {
        HWND curWin = GetForegroundWindow();
        ShowClipsMenu(hWnd, curWin, cManager, FALSE);
    }
    break;
    case WM_CREATE:
    {
        // add the notification icon
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
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, TRUE);

                if (value < MAX_DISPLAY_LOWER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, MAX_DISPLAY_LOWER, TRUE);
                }
                else if (value > MAX_DISPLAY_UPPER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, MAX_DISPLAY_UPPER, TRUE);
                }

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, TRUE);

                if (value >= MAX_DISPLAY_LOWER && value <= MAX_DISPLAY_UPPER)
                {
                    cManager.SetDisplayClips(value);
                    uSettings.SetMaxDisplayClips(GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, TRUE));
                    if (value > uSettings.MaxSavedClips())
                    {
                        SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxSavedClips(), TRUE);
                    }
                }
            }
        }
        break;
        case TXT_MAX_CLIPS_SAVED:
        {
            if (HIWORD(wParam) == EN_KILLFOCUS)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, TRUE);

                if (value < MAX_SAVED_LOWER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, MAX_SAVED_LOWER, TRUE);
                }
                else if (value > MAX_SAVED_UPPER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, MAX_SAVED_UPPER, TRUE);
                }
                else if (value < uSettings.MaxDisplayClips())
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxDisplayClips(), TRUE);
                }

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, TRUE);

                if (value >= MAX_SAVED_LOWER && value <= MAX_SAVED_UPPER && value >= uSettings.MaxDisplayClips())
                {
                    cManager.SetMaxClips(value);
                    uSettings.SetMaxSavedClips(GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, TRUE));
                }
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
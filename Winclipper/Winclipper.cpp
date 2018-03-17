// Winclipper.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "resource.h"
#include <Shlwapi.h>
#include <string>
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

	RunUpdater();

    NONCLIENTMETRICS hfDefault;
    hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
    hFontStd = CreateFontIndirectW(&hfDefault.lfCaptionFont);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCLIPPER, szMainWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCPREVIEW, szPreviewWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCSETTINGS, szSettingsWindowClass, MAX_LOADSTRING);
    RegisterMainClass(hInstance);
    RegisterPreviewWindowClass(hInstance);
    RegisterSettingsWindowClass(hInstance);

    hInst = hInstance; // Store instance handle in our global variable

    if (!InitMainWindow (hInstance, nCmdShow))
    {
        return false;
    }
    if (!InitPreviewWindow(hInstance, nCmdShow))
    {
        return false;
    }
    if (!InitSettingsWindow(hInstance, nCmdShow))
    {
        return false;
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

ATOM RegisterPreviewWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = PreviewWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = szPreviewWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));

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
bool InitMainWindow(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindowExW(0, szMainWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      0, 0, 100, 100, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return false;
   }

   // Still trying to figure out how to not do this. I wish events were a thing.
   mainWnd = hWnd;

   // Adds the main window to the clipboard format listener list.
   AddClipboardFormatListener(hWnd);

   // Register the hotkey that we have stored in settings
   RegisterHotKey(hWnd, ID_REG_HOTKEY, HIBYTE(uSettings.ClipsMenuHotkeyTrl()), LOBYTE(uSettings.ClipsMenuHotkeyTrl()));

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   
   return true;
}

bool InitPreviewWindow(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd = CreateWindowExW(0, szPreviewWindowClass, L"", WS_POPUP,
        0, 0, ScaleX(400), ScaleY(400), mainWnd, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return false;
    }

    previewWnd = hWnd;

    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    return true;
}

// Initializes the settings window. This window contains
// all of the available user settings. For the most part,
// we don't care about the handles of each control. Just make 
// sure that they all have an ID so we can reference them later.
bool InitSettingsWindow(HINSTANCE hInstance, int nCmdShow)
{

    HWND hWnd = CreateWindowExW(0, szSettingsWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, ScaleX(260), ScaleY(284), nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return false;
    }

    // Still trying to figure out how to not do this.
    settingsWnd = hWnd;

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
        if (QueryKeyForValue(hOpened, L"Winclipper") == true)
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
            {
                if (!IsWindowVisible(settingsWnd))
                {
                    ShowWindow(settingsWnd, SW_SHOW);
                }
                SetForegroundWindow(settingsWnd);
            }
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            {
                // wait until finished writing, or give up after 6 seconds (should be long enough)
                int waitUntilFinishedAttempts = 3;
                while (!(cManager.NoPendingClipWrites() && uSettings.NoPendingSettingWrites()) && waitUntilFinishedAttempts > 0)
                {
                    Sleep(2000);
                    waitUntilFinishedAttempts--;
                }
                DestroyWindow(hWnd);
            }
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
            //HWND curWin = GetForegroundWindow();
            ShowClipsMenu(hWnd, cManager, true);
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
        //HWND curWin = GetForegroundWindow();
        ShowClipsMenu(hWnd, cManager, false);
    }
    break;
    case WM_CREATE:
    {
        // Add the notification icon
        if (!AddNotificationIcon(hWnd))
        {
            MessageBox(hWnd,
                L"There was an error adding the notification icon. You might want to try restarting your computer, or reinstalling this application.",
                L"Error adding icon", MB_OK);
            
            return -1;
        }
    }
    break;
    case WM_DESTROY:
    {
        DeleteObject(hFontStd);
        DeleteNotificationIcon();
        PostQuitMessage(0);
    }
    break;
    case WM_INITMENU:
    {
        // keep track of the top level menu to differentiate for display purposes
        topPopupMenu = (HMENU)wParam;
        size_t size = cManager.GetClips().size();
        SelectDefaultMenuItem(size > 1 ? uSettings.Select2ndClip() : false);
    }
    break;
    case WM_INITMENUPOPUP:
    {
        // whenever a submenu appears, we savte it the active popup
        activePopupMenu = (HMENU)wParam;
    }
    break;
    case WM_UNINITMENUPOPUP:
    {
        // when a submenu is destroyed, we set the active popup back to the top
        activePopupMenu = topPopupMenu;
    }
    break;
    case WM_MENUSELECT:
    {
        if (uSettings.ShowPreview())
        {
            // Display preview window when user hovers on a clip
            DWORD flags = HIWORD(wParam);
            if ((flags & MF_HILITE) && !(flags & MF_POPUP))
            {
                int i = LOWORD(wParam);
                if (i != 0 && i <= cManager.GetClips().size())
                {
                    previewClip = cManager.GetClips()[i - 1];

                    int offset = 1;

                    if (activePopupMenu != topPopupMenu)
                    {
                        offset += cManager.DisplayClips();
                    }

                    /* BLACK MAGIC 0_0 */
                    // This gets a MenuItemRect for an item position we know (0),
                    // and then performs a hit-test on the menuItem that sits over.
                    // This might be the same item, or it could be a different one
                    // if the menu has been scrolled. That is our delta value that
                    // can be used to calculate the true offset for the preview.
                    LPRECT hitTestRect = new RECT();
                    int hitTestDelta = 0;
                    if (GetMenuItemRect(hWnd, activePopupMenu, 0, hitTestRect))
                    {
                        LPPOINT hitTest = new POINT({ hitTestRect->left + 1, hitTestRect->top + 1 });
                        hitTestDelta = MenuItemFromPoint(hWnd, activePopupMenu, *hitTest);
                        delete hitTest;
                    }
                    delete hitTestRect;

                    LPRECT menuItemDims = new RECT();

                    if (GetMenuItemRect(hWnd, activePopupMenu, i - offset - hitTestDelta, menuItemDims))
                    {
                        // Let's let our max size be 500
                        long width = ScaleX(500);
                        long height = ScaleY(500);

                        LPRECT previewSize = new RECT({ 0, 0, width, height });
                        MeasureStringMultilineWrap(previewClip, hFontStd, previewSize);

                        if (previewSize->right < width)
                        {
                            width = previewSize->right;
                        }
                        if (previewSize->bottom < height)
                        {
                            remainingTextLines = 0;
                            height = previewSize->bottom;
                        }
                        else
                        {
                            // Get an approximation of the remaining lines to display.
                            remainingTextLines = ((previewSize->bottom - height) / textDrawHeight);

                            previewSize->bottom = height;
                        }
                        
                        CopyRect(previewRect, previewSize);
                        delete previewSize;

                        int xLoc = menuItemDims->right + ScaleX(5);
                        int yLoc = menuItemDims->top;

                        LPRECT desktop = new RECT();
                        GetWindowRect(GetDesktopWindow(), desktop);

                        // These two if's account for the preview going off screen.
                        // their position is precalculated, and if they would move out
                        // of the desktop area, they are recalculated to appear on
                        // the other side of the popupmenu, unless this would make
                        // them go off screen.
                        if (xLoc + width + ScaleX(30) > desktop->right)
                        {
                            int xLocTemp = menuItemDims->left - (width + ScaleX(32));
                            if (xLocTemp > 0)
                            {
                                xLoc = xLocTemp;
                            }
                        }
                        if (yLoc + height + ScaleX(60) > desktop->bottom)
                        {
                            int yLocTemp = menuItemDims->bottom - (height + ScaleY(24));
                            if (yLocTemp > 0)
                            {
                                yLoc = yLocTemp;
                            }
                        }

                        delete desktop;

                        // The two flags at the end ensure that we don't loose focus of the popup menu.
                        RedrawWindow(previewWnd, NULL, NULL, RDW_INVALIDATE);
                        SetWindowPos(previewWnd, HWND_TOPMOST, xLoc, yLoc, width + ScaleX(26), height + ScaleY(24), SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                        ShowWindow(previewWnd, SW_SHOWNA);

                    }
                    delete menuItemDims;
                }
                else
                {
                    ShowWindow(previewWnd, SW_HIDE);
                }
            }
            else
            {
                ShowWindow(previewWnd, SW_HIDE);
            }
        }
    }
    break;
    default:
        // This ensures that if the mouse is moved outside of the popup menu,
        // then the preview window will be hidden. This is achieved by 
        // comparing the previous mouse location against the current, and if
        // they are different, we check to see if they are now selecting a
        // menuItem. If not, then hide the preview window.
        if (IsWindowVisible(previewWnd))
        {
            LPPOINT cPos = new POINT;
            if (GetCursorPos(cPos))
            {
                if (cPos->x != previousMouseLoc->x || cPos->y != previousMouseLoc->y)
                {
                    if (MenuItemFromPoint(hWnd, activePopupMenu, *cPos) == -1)
                    {
                        ShowWindow(previewWnd, SW_HIDE);
                    }

                    previousMouseLoc->x = cPos->x;
                    previousMouseLoc->y = cPos->y;

                }
            }
            delete cPos;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK PreviewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        DeleteObject(hbrBkgnd);
    }
    break;
    case WM_CREATE:
    {
        // This should use the same text measurement that
        // we will ultimately use when rendering final text.
        LPRECT textDims = new RECT({ 0, 0,  ScaleX(800), ScaleY(100) });
        MeasureStringMultilineWrap(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", hFontStd, textDims);
        textDrawHeight = textDims->bottom;
        delete textDims;
    }
    break;
    case WM_PAINT:
    {
        if (previewClip == nullptr)
        {
            return 0;
        }

        PAINTSTRUCT ps;
        HDC hdc;

        hdc = BeginPaint(hWnd, &ps);
        HPEN hpenInitial = static_cast<HPEN>(SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(180, 180, 180))));

        Rectangle(hdc, previewRect->left + ScaleX(6), previewRect->top + ScaleY(6), previewRect->right + ScaleX(20), previewRect->bottom + ScaleY(18));

        LPRECT textRect = new RECT();
        CopyRect(textRect, previewRect);
        textRect->top += ScaleY(10);
        textRect->left += ScaleX(12);
        textRect->right += ScaleX(12);

        SelectObject(hdc, hFontStd);
        SetBkMode(hdc, TRANSPARENT);

        if (remainingTextLines > 0)
        {
            textRect->bottom += ScaleY(10) - textDrawHeight;
            
            LPRECT infoBreakRect = new RECT();
            CopyRect(infoBreakRect, previewRect);
            infoBreakRect->top = infoBreakRect->bottom - textDrawHeight + ScaleY(16);
            infoBreakRect->left += ScaleX(6);
            infoBreakRect->right += ScaleX(20);
            infoBreakRect->bottom += ScaleY(18);

            HBRUSH hbrushInitial = static_cast<HBRUSH>(SelectObject(hdc, CreateSolidBrush(RGB(240, 240, 245))));

            Rectangle(hdc, infoBreakRect->left, infoBreakRect->top, infoBreakRect->right, infoBreakRect->bottom);

            SetTextColor(hdc, RGB(100, 100, 120));
            DrawText(hdc, (L"+ " + std::to_wstring(remainingTextLines) + L" more lines").c_str(), -1, infoBreakRect, DT_WORDBREAK | DT_EXTERNALLEADING | DT_NOPREFIX | DT_EDITCONTROL | DT_CENTER | DT_VCENTER);
            SetTextColor(hdc, BLACK_BRUSH);
            SelectObject(hdc, hbrushInitial);
            delete infoBreakRect;
        }
        else
        {
            textRect->bottom += ScaleY(10);
        }

        SelectObject(hdc, hpenInitial);
        DrawText(hdc, previewClip, -1, textRect, DT_WORDBREAK | DT_EXTERNALLEADING | DT_NOPREFIX | DT_EDITCONTROL);

        EndPaint(hWnd, &ps);
        delete textRect;
        return 0;
    }
    break;
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
            {
                // wait until finished writing, or give up after 6 seconds (should be long enough)
                int waitUntilFinishedAttempts = 3;
                while (!(cManager.NoPendingClipWrites() && uSettings.NoPendingSettingWrites()) && waitUntilFinishedAttempts > 0)
                {
                    Sleep(2000);
                    waitUntilFinishedAttempts--;
                }
                DestroyWindow(mainWnd);
            }
            break;
        case TXT_MAX_CLIPS_DISPLAY:
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

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_DISPLAY, NULL, false);

                if (value >= MAX_DISPLAY_LOWER && value <= MAX_DISPLAY_UPPER)
                {
                    cManager.SetDisplayClips(value);
                    uSettings.SetMaxDisplayClips(value);
                    if (value > uSettings.MaxSavedClips())
                    {
                        SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, value, false);
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
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, false);

                if (value < uSettings.MaxDisplayClips())
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, uSettings.MaxDisplayClips(), false);
                }
                else if (value > MAX_SAVED_UPPER)
                {
                    SetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, MAX_SAVED_UPPER, false);
                }

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, false);

                if (value <= MAX_SAVED_UPPER && value >= uSettings.MaxDisplayClips())
                {
                    cManager.SetMaxClips(value);
                    uSettings.SetMaxSavedClips(GetDlgItemInt(hWnd, TXT_MAX_CLIPS_SAVED, NULL, false));
                }
            }
        }
        break;
        case TXT_MENU_DISP_CHARS:
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

                break; // set focus on the main window again
            }
            else if (HIWORD(wParam) == EN_CHANGE)
            {
                int value = GetDlgItemInt(hWnd, TXT_MENU_DISP_CHARS, NULL, false);

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
        case CHK_SLCT_2ND_CLIP:
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
        }
        break;
        case CHK_SHOW_PREVIEW:
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

// Execute StupidSimpleUpdater
bool RunUpdater()
{
	const int MAX_CMD = 32767;
	wchar_t pPath[MAX_PATH];
	wchar_t commandLine[MAX_CMD];

	GetModuleFileName(0, pPath, MAX_PATH);
	PathRemoveFileSpec(pPath);
	PathAppend(pPath, L"\\StupidSimpleUpdater.exe");
	wcscpy_s(commandLine, pPath);
	wcscat_s(commandLine, L" -appinv");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (CreateProcess(pPath, commandLine, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

// Writes the "run at startup" registry key for Winclipper
bool WriteRegistryRun()
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
bool DeleteRegistryRun()
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

// Get the value of a key in the registry if one exists
bool QueryKeyForValue(HKEY hKey, wchar_t* checkValue)
{
    wchar_t    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
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

    wchar_t * achValue;
    achValue = new wchar_t[MAX_VALUE_NAME];
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
                if (wcscmp(checkValue, achValue) == 0)
                {
                    return true;
                }
            }
        }
    }
    delete[] achValue;
    return false;
}


// Adds the application notification icon to the notifcation area
bool AddNotificationIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.uID = 
    // Load the icon for high DPI.
    LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_WINCLIPPER), LIM_SMALL, &nid.hIcon);
    
    // Show the notification.
    Shell_NotifyIcon(NIM_ADD, &nid);

    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

// Removes the application notification icon from the notifcation area
bool DeleteNotificationIcon()
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.uID = UID_NOTIFYICON;
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}
#pragma once

#define MAX_LOADSTRING	        100
#define MAX_VALUE_NAME          16383
#define ID_REG_HOTKEY	        100
#define UID_NOTIFYICON          2180847866      // random UID for the notifyicon

#define LBL_MAX_CLIPS_DISPLAY   200
#define TXT_MAX_CLIPS_DISPLAY   201
#define UD_MAX_CLIPS_DISPLAY    202

#define LBL_MAX_CLIPS_SAVED     203
#define TXT_MAX_CLIPS_SAVED     204
#define UD_MAX_CLIPS_SAVED      205

#define LBL_MENU_DISP_CHARS     206
#define TXT_MENU_DISP_CHARS     207
#define UD_MENU_DISP_CHARS      208

#define CHK_RUN_AT_STARTUP      209
#define CHK_SAVE_TO_DISK        210

#define LBL_SHOW_CLIPS_HOTK     211
#define HKY_SHOW_CLIPS_MENU     212

// Global Variables:
HINSTANCE hInst;                                    // current instance
HWND mainWnd;
HWND settingsWnd;
WCHAR szTitle[MAX_LOADSTRING];                      // The title bar text
WCHAR szMainWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szSettingsWindowClass[MAX_LOADSTRING];        // the settings window class name
UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;       // user defined callback for notfiyicon actions
UserSettings uSettings;                             // user settings manager
ClipsManager cManager { uSettings.MaxDisplayClips(), uSettings.MaxSavedClips(), uSettings.MenuDisplayChars() }; // clips manager

// Forward declarations of functions included in this code module:
ATOM                    RegisterMainClass(HINSTANCE hInstance);
ATOM                    RegisterSettingsWindowClass(HINSTANCE hInstance);
BOOL                    InitMainWindow(HINSTANCE, int);
BOOL                    InitSettingsWindow(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        SettingsWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
BOOL                    WriteRegistryRun();
BOOL                    DeleteRegistryRun();
BOOL                    QueryKeyForValue(HKEY hKey, TCHAR* checkValue);
BOOL                    AddNotificationIcon(HWND hWnd);
BOOL                    DeleteNotificationIcon();
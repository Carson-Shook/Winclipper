#pragma once

#define MAX_LOADSTRING          100
#define MAX_VALUE_NAME          16383
#define ID_REG_HOTKEY           100
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
#define CHK_SLCT_2ND_CLIP       211
#define CHK_SHOW_PREVIEW        212

#define LBL_SHOW_CLIPS_HOTK     213
#define HKY_SHOW_CLIPS_MENU     214

#define TXT_CLIP_PREVIEW        215
#define WND_PREVIEW             216

// Global Variables:
HINSTANCE hInst;                                    // current instance
HWND mainWnd;
HWND previewWnd;
HWND previewTextBox;
HWND settingsWnd;
HMENU topPopupMenu;
HMENU activePopupMenu;
wchar_t szTitle[MAX_LOADSTRING];                    // The title bar text
wchar_t szMainWindowClass[MAX_LOADSTRING];          // the main window class name
wchar_t szPreviewWindowClass[MAX_LOADSTRING];       // the text preview window
wchar_t szSettingsWindowClass[MAX_LOADSTRING];      // the settings window class name
UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;       // user defined callback for notfiyicon actions
UserSettings uSettings;                             // user settings manager
ClipsManager cManager { uSettings.MaxDisplayClips(), uSettings.MaxSavedClips(), uSettings.MenuDisplayChars(), uSettings.SaveToDisk() }; // clips manager

HBRUSH hbrBkgnd;
HFONT hFontStd;
long textDrawHeight;
unsigned long long remainingTextLines = 0; // need to account for theoretical maximum NTFS file size
LPRECT previewRect = new RECT({ 0, 0, ScaleX(500), ScaleY(500) });
wchar_t * previewClip = nullptr;
LPPOINT previousMouseLoc = new POINT;

// Forward declarations of functions included in this code module:
ATOM                    RegisterMainClass(HINSTANCE hInstance);
ATOM                    RegisterPreviewWindowClass(HINSTANCE hInstance);
ATOM                    RegisterSettingsWindowClass(HINSTANCE hInstance);
bool                    InitMainWindow(HINSTANCE, int);
bool                    InitPreviewWindow(HINSTANCE, int);
bool                    InitSettingsWindow(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        PreviewWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        SettingsWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
bool					RunUpdater();
bool                    WriteRegistryRun();
bool                    DeleteRegistryRun();
bool                    QueryKeyForValue(HKEY hKey, wchar_t* checkValue);
bool                    AddNotificationIcon(HWND hWnd);
bool                    DeleteNotificationIcon();
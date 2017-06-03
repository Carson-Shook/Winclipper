#pragma once

#define MAX_LOADSTRING	        100
#define ID_REG_HOTKEY	        100
#define UID_NOTIFYICON          2180847866      // random UID for the notifyicon
#define LBL_MAX_CLIPS_DISPLAY   200
#define LBL_MAX_CLIPS_SAVED     201
#define TXT_MAX_CLIPS_DISPLAY   202
#define TXT_MAX_CLIPS_SAVED     203
#define UD_MAX_CLIPS_DISPLAY    204
#define UD_MAX_CLIPS_SAVED      205

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;   // user defined callback for notfiyicon actions
ClipsManager cManager;                          // clips manager
UserSettings uSettings;                         // user settings manager

// Forward declarations of functions included in this code module:
ATOM                    MyRegisterClass(HINSTANCE hInstance);
BOOL                    InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
BOOL                    AddNotificationIcon(HWND hWnd);
BOOL                    DeleteNotificationIcon();
void                    ShowContextMenu(HWND hWnd, POINT pt);
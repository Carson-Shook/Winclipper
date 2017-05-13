#pragma once

#define MAX_LOADSTRING	100
#define ID_REG_HOTKEY	100
#define UID_NOTIFYICON  2180847866              // random UID for the notifyicon

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
ClipsManager cManager;
UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;   // user defined callback for notfiyicon actions

// Forward declarations of functions included in this code module:
ATOM                    MyRegisterClass(HINSTANCE hInstance);
BOOL                    InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
BOOL                    AddNotificationIcon(HWND hWnd);
BOOL                    DeleteNotificationIcon();
void                    ShowContextMenu(HWND hWnd, POINT pt);
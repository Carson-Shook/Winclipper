#pragma once
#include "resource.h"
#include "string.h"
#include "ClipsManager.h"
#include "PreviewWindow.h"
#include "UserSettings.h"
#include "SettingsWindow.h"

#define UID_NOTIFYICON          2180847866      // random UID for the notifyicon

class MainWindow
{
private:
	bool						ninKeypressLocked = false;
	HINSTANCE					hInst;										// current instance
	wchar_t						szMainWindowClass[MAX_LOADSTRING];          // the main window class name
	wchar_t						szTitle[MAX_LOADSTRING];                    // The title bar text
	UserSettings & 				uSettings;
	SettingsWindow &			settingsWindow;
	ClipsManager * 				cManager = nullptr;
	PreviewWindow * 			previewWindow = nullptr;
	HMENU						topPopupMenu;
	HMENU						activePopupMenu;
	static UINT	const			WMAPP_NOTIFYCALLBACK = WM_APP + 1;			// user defined callback for notfiyicon actions
	LPPOINT						previousMouseLoc = new POINT;

	HWND						windowHandle;
	
	ATOM						RegisterMainWindowClass(HINSTANCE hInstance);
	static LRESULT CALLBACK		MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK		About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT						WmCommandIdmClearClips(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmSettings(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmAbout(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmExit(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfCmenuHotkeyChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfMaxDisplayChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfMaxSavedChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfMenuCharsChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfSaveToDiskChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfSaveImagesChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandNtfMaxcacheMBytesChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmappNCallNinKeySelect(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmappNCallWmLButtonDblClk(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmappNCallWmContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmClipboardUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmHotkey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmInitMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmMenuRButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmInitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmUninitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmMenuSelect(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WndProcDefault(HWND hWnd, WPARAM wParam, LPARAM lParam);

	bool						AddNotificationIcon(HWND hWnd, HINSTANCE hInstance);
	bool						DeleteNotificationIcon();
public:
								MainWindow(HINSTANCE hInstance, UserSettings & userSettings, SettingsWindow & settingsWindow);
								~MainWindow();

	bool					    InitMainWindow(HINSTANCE hInstance);
	HWND						GetHandle();

};


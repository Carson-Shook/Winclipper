#pragma once
#include "resource.h"
#include "string.h"
#include "ClipsManager.h"
#include "PreviewWindow.h"
#include "UserSettings.h"
#include "SettingsWindow.h"

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

class MainWindow
{
private:
	HINSTANCE					hInst;										// current instance
	wchar_t						szMainWindowClass[MAX_LOADSTRING];          // the main window class name
	wchar_t						szTitle[MAX_LOADSTRING];                    // The title bar text
	UserSettings & 				uSettings;
	SettingsWindow &			settingsWindow;
	ClipsManager * 				cManager;
	PreviewWindow * 			previewWindow;
	HWND 						previewWnd;
	HMENU						topPopupMenu;
	HMENU						activePopupMenu;
	static UINT	const			WMAPP_NOTIFYCALLBACK = WM_APP + 1;			// user defined callback for notfiyicon actions
	LPPOINT						previousMouseLoc = new POINT;

	HFONT						hFontStd;
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
	//LRESULT						WmCommandNtfShowPreviewChanged(HWND hWnd, WPARAM wParam, LPARAM lParam); // don't really need this yet.
	//LRESULT						WmCommandNtfSlctSecondClipChanged(HWND hWnd, WPARAM wParam, LPARAM lParam); // same
	LRESULT						WmappNCallWmLButtonDblClk(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmappNCallWmContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmClipboardUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmHotkey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmInitMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
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


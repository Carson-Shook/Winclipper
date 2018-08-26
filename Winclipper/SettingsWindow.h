#pragma once
#include "UserSettings.h"

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


class SettingsWindow : public Notify
{
private:
	wchar_t						szSettingsWindowClass[MAX_LOADSTRING];      // the settings window class name
	wchar_t						szTitle[MAX_LOADSTRING];                    // The title bar text
	UserSettings &				uSettings;
	
	HFONT						hFontStd;
	HWND						windowHandle;

	ATOM						RegisterSettingsWindowClass(HINSTANCE hInstance);
	static LRESULT CALLBACK		SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmAbout(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmExit();
	LRESULT						WmCommandTxtMaxClipsDisplay(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandTxtMaxClipsSaved(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandTxtMenuDispChars(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkRunAtStartup(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkSaveToDisk(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkSlct2ndClip(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkShowPreview(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandHkyShowClipsMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmLbuttondown(HWND hWnd);
	LRESULT						WmClose(HWND hWnd);

	bool						WriteRegistryRun();
	bool						DeleteRegistryRun();
public:
								SettingsWindow(HINSTANCE hInstance, UserSettings & userSettings);
								~SettingsWindow();

	bool						InitSettingsWindow(HINSTANCE, HWND);
	HWND						GetHandle();
};


#pragma once
#include "resource.h"
#include "string.h"
#include "ControlUtilities.h"
#include "RegistryUtilities.h"
#include "Notify.h"
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
#define CHK_SAVE_IMAGES	        213

#define LBL_SHOW_CLIPS_HOTK     214
#define HKY_SHOW_CLIPS_MENU     215

#define LBL_MAX_IMG_CACHE_MB    216
#define TXT_MAX_IMG_CACHE_MB    217
#define UD_MAX_IMG_CACHE_MB     218


class SettingsWindow : public Notify
{
private:
	wchar_t						szSettingsWindowClass[MAX_LOADSTRING];      // the settings window class name
	wchar_t						szTitle[MAX_LOADSTRING];                    // The title bar text
	
	HFONT						hFontStd;
	HWND						windowHandle = nullptr;

	ATOM						RegisterSettingsWindowClass(HINSTANCE hInstance);
	static LRESULT CALLBACK		SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmAbout(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandIdmExit();
	LRESULT						WmCommandIdmRecreateThumbnails();
	LRESULT						WmCommandIdmClearClips();
	LRESULT						WmCommandTxtMaxClipsDisplay(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandTxtMaxClipsSaved(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandTxtMenuDispChars(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkRunAtStartup(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkSaveToDisk(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkSlct2ndClip(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkShowPreview(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandChkSaveImages(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandTxtMaxImageCacheMb(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCommandHkyShowClipsMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT						WmLbuttondown(HWND hWnd);
	LRESULT						WmClose(HWND hWnd);

	bool						WriteRegistryRun();
	bool						DeleteRegistryRun();
public:
								SettingsWindow(HINSTANCE hInstance);
								~SettingsWindow();

	bool						InitSettingsWindow(HINSTANCE hInstance);
	HWND						GetHandle() noexcept;
};


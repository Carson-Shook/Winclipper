#include "stdafx.h"
#include "ControlUtilities.h"

int _InitX() noexcept
{
	int dpiX = 0;
	HDC hdc = GetDC(nullptr);
	if (hdc)
	{
		dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		ReleaseDC(nullptr, hdc);
	}
	return dpiX;
}

int _InitY() noexcept
{
	int dpiY = 0;
	HDC hdc = GetDC(nullptr);
	if (hdc)
	{
		dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(nullptr, hdc);
	}
	return dpiY;
}

int _dpiX = _InitX();
int _dpiY = _InitY();

int ScaleX(int x) noexcept { return MulDiv(x, _dpiX, 96); }
int ScaleY(int y) noexcept { return MulDiv(y, _dpiY, 96); }
float ScaleX(float x) noexcept { return (x * _dpiX) / 96.0F; }
float ScaleY(float y) noexcept { return (y * _dpiY) / 96.0F; }

// Creates an instance of a hotkey control with a specified width and height 
HWND AddHotkeyCtrl(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, UINT_PTR id)
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_HOTKEY_CLASS;

	InitCommonControlsEx(&icex);

	HWND hControl = CreateWindowEx(0,    //Extended window styles.
		HOTKEY_CLASS,
		L"",
		WS_CHILD | WS_VISIBLE,
		ScaleX(x), ScaleY(y),
		ScaleX(width), ScaleY(height),
		hWnd,
		(HMENU)id,
		hIn,
		nullptr);

	SendMessageW(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));

	SetFocus(hControl);

	SendMessageW(hControl,
		HKM_SETRULES,
		(WPARAM)HKCOMB_NONE | HKCOMB_S,   // invalid key combinations 
		MAKELPARAM(HOTKEYF_ALT, 0));       // add ALT to invalid 

	SendMessageW(hControl,
		HKM_SETHOTKEY,
		MAKEWORD(0x41, HOTKEYF_CONTROL | HOTKEYF_ALT),
		0);

	return hControl;
}



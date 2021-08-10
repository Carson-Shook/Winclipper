// The base class of all control types
#include "Control.h"

int Controls::Control::InitDpiX()
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

int Controls::Control::InitDpiY()
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

int Controls::Control::DpiScaleX(int x) noexcept
{
	return MulDiv(x, dpiX, 96);
}

int Controls::Control::DpiScaleY(int y) noexcept
{
	return MulDiv(y, dpiY, 96);
}

// Draw text to a fit within a rectangle, and then measure
// the resulting rectangle to determine how much space is
// required to draw the text in a dialog.
void Controls::Control::MeasureString(LPCWSTR text, HFONT font, LPRECT rect)
{
	HDC hDC = GetDC(nullptr);
	SelectObject(hDC, font);
	DrawTextW(hDC, text, -1, rect, DT_CALCRECT);
	ReleaseDC(nullptr, hDC);
	return;
}

bool Controls::Control::Create()
{
	// This method should probably always be overridden
	// with a custom implementation. Base should not be called.
	// Make sure to set exists = true before returning.
	handle = CreateWindowExW(WS_EX_LEFT,
		WC_STATIC,
		Text,
		WS_CHILD | WS_VISIBLE,
		DpiScaleX(X), DpiScaleY(Y),
		DpiScaleX(Width), DpiScaleY(Height),
		parentHandle,
		(HMENU)id,
		hInstance,
		nullptr);

	return handle != nullptr;
}

Controls::Control::Control()
{
}

Controls::Control::Control(HWND hWnd, UINT_PTR id, HINSTANCE hInstance)
{
	parentHandle = hWnd;
	dpiX = InitDpiX();
	dpiY = InitDpiY();
	Control::id = id;
	Control::hInstance = hInstance;
}

void Controls::Control::PerformLayout()
{
	if (!LayoutSuspended())
	{
		if (!exists)
		{
			exists = Create();
		}
		else
		{
			MoveWindow(handle, DpiScaleX(X), DpiScaleY(Y), DpiScaleX(Width), DpiScaleY(Height), true);
			SendMessageW(handle, WM_SETTEXT, NULL, (LPARAM)Control::Text);
		}
		SendMessageW(handle, WM_SETFONT, (WPARAM)Control::font, MAKELPARAM(false, 0));
	}
}

void Controls::Control::SuspendLayout()
{
	layoutSuspended = true;
}

void Controls::Control::ResumeLayout(bool layoutImmediately)
{
	layoutSuspended = false;

	if (layoutImmediately)
	{
		PerformLayout();
	}
}

HWND Controls::Control::Handle()
{
	return handle;
}

HWND Controls::Control::ParentHandle()
{
	return parentHandle;
}

bool Controls::Control::Exists()
{
	return exists;
}

bool Controls::Control::LayoutSuspended()
{
	return layoutSuspended;
}

void Controls::Control::SetEnabled(bool enabled)
{
	EnableWindow(handle, enabled);
}

void Controls::Control::SetText(LPCWSTR text)
{
	Control::Text = text;
	SendMessageW(handle, WM_SETTEXT, NULL, (LPARAM)Control::Text);
}

void Controls::Control::SetFont(HFONT font)
{
	Control::font = font;
	SendMessageW(handle, WM_SETFONT, (WPARAM)Control::font, MAKELPARAM(false, 0));
}

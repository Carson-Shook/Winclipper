#include "Edit.h"
bool Controls::Edit::Create()
{
	DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT;

	if (ReadOnly)
	{
		style = style | ES_READONLY;
	}
	if (Multiline)
	{
		style = style | ES_MULTILINE;
	}
	if (VerticalScrolling)
	{
		style = style | ES_AUTOVSCROLL;
	}
	if (HorizontalScrolling)
	{
		style = style | ES_AUTOHSCROLL;
	}
	if (Numeric)
	{
		style = style | ES_NUMBER;
	}

	handle = CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE,
		WC_EDIT,
		text,
		style,
		DpiScaleX(x), DpiScaleY(y),
		DpiScaleX(width), DpiScaleY(height),
		parentHandle,
		(HMENU)id,
		hInstance,
		nullptr);

	SendMessageW(handle, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));
	if (VerticalScrolling)
	{
		ShowScrollBar(handle, SB_VERT, true);
	}
	if (HorizontalScrolling)
	{
		ShowScrollBar(handle, SB_HORZ, true);
	}

	return handle != nullptr;
}

Controls::Edit::Edit()
	: Control{}
{ }

Controls::Edit::Edit(HWND hWnd, UINT_PTR id, HINSTANCE hInstance)
	: Control{ hWnd, id, hInstance }
{ }
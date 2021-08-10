#include "Label.h"

bool Controls::Label::Create()
{
	RECT boxsize = { 0, 0, 0, 0 };
	int finalWidth = 0;
	int finalHeight = 0;

	if (AutoSize)
	{
		MeasureString(text, font, &boxsize);
		finalWidth = boxsize.right;
		finalHeight = boxsize.bottom;
	}
	else
	{
		finalWidth = DpiScaleX(width);
		finalHeight = DpiScaleX(height);
	}

	handle = CreateWindowExW(WS_EX_LEFT,
		WC_STATIC,
		text,
		WS_CHILD | WS_VISIBLE,
		DpiScaleX(x), DpiScaleY(y),
		finalWidth, finalHeight,
		parentHandle,
		(HMENU)id,
		hInstance,
		nullptr);

	return handle != nullptr;
}

Controls::Label::Label()
	: Control{}
{ }

Controls::Label::Label(HWND hWnd, UINT_PTR id, HINSTANCE hInstance)
	: Control{ hWnd, id, hInstance }
{ }

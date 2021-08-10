#include "Font.h"

HFONT Controls::Font::GetSystemFont()
{
	NONCLIENTMETRICS hfDefault;
	hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
	return CreateFontIndirectW(&hfDefault.lfMessageFont);
}

HFONT Controls::Font::GetFontByName(LPCWSTR fontName, int size)
{
	HDC hDC = GetDC(nullptr);
	return CreateFontW(
		-MulDiv(size, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0,
		0,
		0,
		FW_DONTCARE,
		false,
		false,
		false,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		fontName
	);
}

HFONT Controls::Font::SystemDefault = GetSystemFont();
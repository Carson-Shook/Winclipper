#pragma once
#include "framework.h"

namespace Controls
{
	static class Font
	{
	private:
		static HFONT GetSystemFont();
		static HFONT GetFontByName(LPCWSTR fontName, int size);
	public:
		static HFONT SystemDefault;
	};
}
#include "Checkbox.h"
#include "uxtheme.h"
bool Controls::Checkbox::Create()
{
	RECT boxsize = { 0, 0, 0, 0 };
	int finalWidth = 0;
	int finalHeight = 0;

	int checkboxWidth = GetCheckboxWidth();

	if (AutoSize)
	{
		MeasureString(text, font, &boxsize);
		finalWidth = boxsize.right + checkboxWidth;
		finalHeight = checkboxWidth > boxsize.bottom ? checkboxWidth : boxsize.bottom;
	}
	else
	{
		finalWidth = DpiScaleX(width);
		finalHeight = DpiScaleX(height);
	}


	handle = CreateWindowExW(WS_EX_LEFT | WS_EX_NOACTIVATE,
		WC_BUTTON,
		text,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_CHECKBOX,
		DpiScaleX(x), DpiScaleY(y),
		finalWidth, finalHeight,
		parentHandle,
		(HMENU)id,
		hInstance,
		nullptr);

	if (handle != nullptr && UseBasicTheme)
	{
		SetWindowTheme(handle, nullptr, L" ");
	}

	return handle != nullptr;
}

bool Controls::Checkbox::Value()
{
	return IsDlgButtonChecked(parentHandle, id) == BST_CHECKED;
}

void Controls::Checkbox::SetValue(bool value)
{
	CheckDlgButton(parentHandle, id, value ? BST_CHECKED : BST_UNCHECKED);
}

Controls::Checkbox::Checkbox()
	: Control{}
{ }

Controls::Checkbox::Checkbox(HWND hWnd, UINT_PTR id, HINSTANCE hInstance)
	: Control{ hWnd, id, hInstance }
{ }

int Controls::Checkbox::GetCheckboxWidth()
{
	// Will return 0 if not found
	return GetSystemMetrics(SM_CXMENUCHECK);
}

#include "UpDown.h"

bool Controls::UpDown::Create()
{
	handle = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
		UPDOWN_CLASS,
		nullptr,
		WS_CHILDWINDOW | WS_VISIBLE
		| UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
		0, 0,
		0, 0,         // Set to zero to automatically size to fit the buddy window.
		parentHandle,
		(HMENU)id,
		nullptr,
		nullptr);

	return handle != nullptr;
}

Controls::UpDown::UpDown()
	: Control{}
{ }

Controls::UpDown::UpDown(HWND hWnd, UINT_PTR id, HINSTANCE hInstance)
	: Control{ hWnd, id, hInstance }
{ }

void Controls::UpDown::PerformCustomLayout()
{
	SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(maxValue, minValue));    // Sets the controls direction 
}

int Controls::UpDown::MinValue()
{
	return minValue;
}

void Controls::UpDown::SetMinValue(int value)
{
	minValue = value;
	if (!LayoutSuspended())
	{
		SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(maxValue, minValue));
	}
}

int Controls::UpDown::MaxValue()
{
	return maxValue;
}

void Controls::UpDown::SetMaxValue(int value)
{
	maxValue = value;
	if (!LayoutSuspended())
	{
		SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(maxValue, minValue));
	}
}

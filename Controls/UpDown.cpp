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

	SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(MaxValue, MinValue));    // Sets the controls direction 

	return handle != nullptr;
}

Controls::UpDown::UpDown()
	: Control{}
{ }

Controls::UpDown::UpDown(HWND hWnd, UINT_PTR id, HINSTANCE hInstance)
	: Control{ hWnd, id, hInstance }
{ }

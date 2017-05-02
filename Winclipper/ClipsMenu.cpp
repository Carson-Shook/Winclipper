#include "stdafx.h"
#include "ClipsMenu.h"

int ShowClipsMenu(HWND hWnd, HWND curWin)
{
	if (hWnd != NULL && curWin != NULL)
	{
		SetForegroundWindow(hWnd);

		LPPOINT cPos = new POINT;
		if (!GetCursorPos(cPos))
		{
			return ERROR;
		}

		HMENU menu = CreatePopupMenu();
		AppendMenu(menu, MF_STRING, 999, L"TEST");
		if (TrackPopupMenu(
			menu,
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
			(*cPos).x,
			(*cPos).y,
			0,
			hWnd,
			NULL
		))
		{

			SetForegroundWindow(curWin);
			Sleep(10);
			if (!OpenClipboard(curWin))
				return FALSE;
			HANDLE psClipboardData = GetClipboardData(CF_UNICODETEXT);
			if (psClipboardData != NULL)
			{
				wchar_t * data = reinterpret_cast<wchar_t*>(GlobalLock(psClipboardData));
				wchar_t * derefData = _wcsdup(data);

				SendPasteInput();

				GlobalUnlock(psClipboardData);
			}
			CloseClipboard();
		}
		SetForegroundWindow(curWin);
		PostMessage(hWnd, WM_NULL, 0, 0);
	}
}

void SendPasteInput(void)
{
	INPUT inputCtrlKey;
	INPUT inputVKey;

	inputCtrlKey.type = INPUT_KEYBOARD;
	inputCtrlKey.ki.wVk = VK_CONTROL;
	inputCtrlKey.ki.wScan = 0;
	inputCtrlKey.ki.time = 1;
	inputCtrlKey.ki.dwFlags = 0;
	inputCtrlKey.ki.dwExtraInfo = 0;

	inputVKey.type = INPUT_KEYBOARD;
	inputVKey.ki.wVk = 0x56;
	inputVKey.ki.wScan = 0;
	inputVKey.ki.time = 2;
	inputVKey.ki.dwFlags = 0;
	inputVKey.ki.dwExtraInfo = 0;

	INPUT ip[] = { inputCtrlKey, inputVKey };

	SendInput(2, ip, sizeof(INPUT));

	ip[0].ki.dwFlags = KEYEVENTF_KEYUP;
	ip[1].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(2, ip, sizeof(INPUT));

	return;
}

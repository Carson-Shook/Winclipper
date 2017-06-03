#include "stdafx.h"
#include "resource.h"
#include "ControlUtilities.h"

RECT MeasureString(LPCWSTR text, HFONT font)
{
    HDC hDC = GetDC(NULL);
    SelectObject(hDC, font);
    RECT r = { 0, 0, 0, 0 };
    DrawText(hDC, text, -1, &r, DT_CALCRECT);
    return r;
}

HWND AddLabel(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, int id)
{
    RECT boxsize = MeasureString(text, font);

    HWND hControl = CreateWindowExW(WS_EX_LEFT, WC_STATIC, NULL, WS_CHILD | WS_VISIBLE, x, y, boxsize.right, boxsize.bottom, hWnd, (HMENU)id, hIn, nullptr);
    SendMessage(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
    SetWindowText(hControl, text);

    return hControl;
}

HWND CreateUpDnBuddy(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, int max, int id)
{
    HWND hControl = CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE,    //Extended window styles.
        WC_EDIT,
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE    // Window styles.
        | ES_NUMBER | ES_LEFT,                     // Edit control styles.
        x, y,
        80, 30,
        hWnd,
        (HMENU)id,
        hIn,
        NULL);

    SendMessage(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
    return (hControl);
}

HWND CreateUpDnCtl(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, int max, int id)
{
    HWND hControl = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
        UPDOWN_CLASS,
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE
        | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
        0, 0,
        0, 0,         // Set to zero to automatically size to fit the buddy window.
        hWnd,
        (HMENU)id,
        NULL,
        NULL);

    SendMessage(hControl, UDM_SETRANGE, 0, MAKELPARAM(max, min));    // Sets the controls direction 
                                                                     // and range.
    return (hControl);
}


VOID AddSpinner(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, int max, int udId, int txtId)
{
    CreateUpDnBuddy(hWnd, font, x, y, hIn, min, max, txtId);
    CreateUpDnCtl(hWnd, font, x, y, hIn, min, max, udId);

    return;
}



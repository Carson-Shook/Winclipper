#include "stdafx.h"
#include "resource.h"
#include "ControlUtilities.h"

int _InitX()
{
    int dpiX;
    HDC hdc = GetDC(NULL);
    if (hdc)
    {
        dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);
    }
    else
    {
        dpiX = 0;
    }
    return dpiX;
}

int _InitY()
{
    int dpiY;
    HDC hdc = GetDC(NULL);
    if (hdc)
    {
        dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(NULL, hdc);
    }
    else
    {
        dpiY = 0;
    }
    return dpiY;
}

int _dpiX = _InitX();
int _dpiY = _InitY();

int ScaleX(int x) { return MulDiv(x, _dpiX, 96); }
int ScaleY(int y) { return MulDiv(y, _dpiY, 96); }

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
    
    HWND hControl = CreateWindowExW(WS_EX_LEFT,
        WC_STATIC,
        text,
        WS_CHILD | WS_VISIBLE,
        ScaleX(x), ScaleY(y),
        boxsize.right, boxsize.bottom,
        hWnd,
        (HMENU)id,
        hIn,
        NULL);
    SendMessage(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
    //SetWindowText(hControl, text);

    return hControl;
}

HWND AddCheckbox(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, int id)
{
    RECT boxsize = MeasureString(text, font);

    HWND hControl = CreateWindowExW(WS_EX_LEFT,
        WC_BUTTON,
        text,
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        ScaleX(x), ScaleY(y),
        boxsize.right + ScaleX(20), boxsize.bottom,
        hWnd,
        (HMENU)id,
        hIn,
        nullptr);
    SendMessage(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
    //SetWindowText(hControl, text);

    return hControl;
}

HWND CreateUpDnBuddy(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int id)
{
    HWND hControl = CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE,    //Extended window styles.
        WC_EDIT,
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP   // Window styles.
        | ES_NUMBER | ES_LEFT,                     // Edit control styles.
        ScaleX(x), ScaleY(y),
        ScaleX(50), ScaleY(19),
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
    CreateUpDnBuddy(hWnd, font, x, y, hIn, txtId);
    CreateUpDnCtl(hWnd, font, x, y, hIn, min, max, udId);

    return;
}

HWND AddHotkeyCtrl(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, int id)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_HOTKEY_CLASS;

    InitCommonControlsEx(&icex);

    HWND hControl = CreateWindowEx(0,    //Extended window styles.
        HOTKEY_CLASS,
        _T(""),
        WS_CHILD | WS_VISIBLE,
        ScaleX(x), ScaleY(y),
        ScaleX(width), ScaleY(height),
        hWnd,
        (HMENU)id,
        hIn,
        NULL);

    SendMessage(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

    SetFocus(hControl);

    SendMessage(hControl,
        HKM_SETRULES,
        (WPARAM)HKCOMB_NONE | HKCOMB_S,   // invalid key combinations 
        MAKELPARAM(HOTKEYF_ALT, 0));       // add ALT to invalid 

    SendMessage(hControl,
        HKM_SETHOTKEY,
        MAKEWORD(0x41, HOTKEYF_CONTROL | HOTKEYF_ALT),
        0);

    return hControl;
}



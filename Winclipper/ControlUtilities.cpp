#include "stdafx.h"
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
float ScaleX(float x) { return (x * _dpiX) / 96.0F; }
float ScaleY(float y) { return (y * _dpiY) / 96.0F; }

// Draw text to a fit within a rectangle, and then measure
// the resulting rectangle to determine how much space is
// required to draw the text in a dialog.
void MeasureString(LPCWSTR text, HFONT font, LPRECT rect)
{
    HDC hDC = GetDC(NULL);
    SelectObject(hDC, font);
    DrawTextW(hDC, text, -1, rect, DT_CALCRECT);
    ReleaseDC(NULL, hDC);
    return;
}

// Creates an instance of a label object with a specified font and custom text.
HWND AddLabel(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, UINT_PTR id)
{
    LPRECT boxsize = new RECT();
    MeasureString(text, font, boxsize);
    
    HWND hControl = CreateWindowExW(WS_EX_LEFT,
        WC_STATIC,
        text,
        WS_CHILD | WS_VISIBLE,
        ScaleX(x), ScaleY(y),
        (*boxsize).right, (*boxsize).bottom,
        hWnd,
        (HMENU)id,
        hIn,
        NULL);
    SendMessageW(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));
    //SetWindowText(hControl, text);
    delete boxsize;
    return hControl;
}

// Creates an instance of an edit object with a specified font and specific dimensions, 
// as well as the optional styles READONLY, MULTILINE, AUTOVSCROLL, and AUTOHSCROLL.
HWND AddEdit(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, LPCWSTR text, UINT_PTR id, bool readOnly, bool multiLine, bool vScroll, bool hScroll)
{
    DWORD style = WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT;

    if (readOnly)
    {
        style = style | ES_READONLY;
    }
    if (multiLine)
    {
        style = style | ES_MULTILINE;
    }
    if (vScroll)
    {
        style = style | ES_AUTOVSCROLL;
    }
    if (hScroll)
    {
        style = style | ES_AUTOHSCROLL;
    }

    HWND hControl = CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE,    //Extended window styles.
        WC_EDIT,
        text,
        style,
        ScaleX(x), ScaleY(y),
        ScaleX(width), ScaleY(height),
        hWnd,
        (HMENU)id,
        hIn,
        NULL);

    SendMessageW(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));
    if (vScroll)
    {
        ShowScrollBar(hControl, SB_VERT, true);
    }
    if (hScroll)
    {
        ShowScrollBar(hControl, SB_HORZ, true);
    }
    return (hControl);
}

// Creates an instance of a checkbox object with a specified font and custom text for the associated label.
HWND AddCheckbox(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, UINT_PTR id)
{
    LPRECT boxsize = new RECT();
    MeasureString(text, font, boxsize);

    HWND hControl = CreateWindowExW(WS_EX_LEFT,
        WC_BUTTON,
        text,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_CHECKBOX,
        ScaleX(x), ScaleY(y),
        (*boxsize).right + ScaleX(20), (*boxsize).bottom,
        hWnd,
        (HMENU)id,
        hIn,
        nullptr);

    SendMessageW(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));
    delete boxsize;
    return hControl;
}

// Creates an instance of an edit control that will be used in a spinner control
HWND CreateUpDnBuddy(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, UINT_PTR id)
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

    SendMessageW(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));
    return (hControl);
}

// Creates an instance of an UpDown control that will be used in a spinner control
HWND CreateUpDnCtl(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, UINT_PTR max, UINT_PTR id)
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

    SendMessageW(hControl, UDM_SETRANGE, 0, MAKELPARAM(max, min));    // Sets the controls direction 
                                                                     // and range.
    return (hControl);
}

// Creates an instance of a spinner control with a specified font and range
VOID AddSpinner(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, int max, UINT_PTR udId, UINT_PTR txtId)
{
    CreateUpDnBuddy(hWnd, font, x, y, hIn, txtId);
    CreateUpDnCtl(hWnd, font, x, y, hIn, min, max, udId);

    return;
}

// Creates an instance of a hotkey control with a specified width and height 
HWND AddHotkeyCtrl(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, UINT_PTR id)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_HOTKEY_CLASS;

    InitCommonControlsEx(&icex);

    HWND hControl = CreateWindowEx(0,    //Extended window styles.
        HOTKEY_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE,
        ScaleX(x), ScaleY(y),
        ScaleX(width), ScaleY(height),
        hWnd,
        (HMENU)id,
        hIn,
        NULL);

    SendMessageW(hControl, WM_SETFONT, (WPARAM)font, MAKELPARAM(false, 0));

    SetFocus(hControl);

    SendMessageW(hControl,
        HKM_SETRULES,
        (WPARAM)HKCOMB_NONE | HKCOMB_S,   // invalid key combinations 
        MAKELPARAM(HOTKEYF_ALT, 0));       // add ALT to invalid 

    SendMessageW(hControl,
        HKM_SETHOTKEY,
        MAKEWORD(0x41, HOTKEYF_CONTROL | HOTKEYF_ALT),
        0);

    return hControl;
}



#include "stdafx.h"
#include "PreviewWindow.h"

ATOM PreviewWindow::RegisterPreviewWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PreviewWindow::PreviewWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szPreviewWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINCLIPPER));

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK PreviewWindow::PreviewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// We want to capture a pointer to the current instance of the class
	// so we can call non-static methods from a static context.
	PreviewWindow * pThis;

	if (message == WM_NCCREATE)
	{
		pThis = static_cast<PreviewWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if (!SetWindowLongPtr(hWnd, AGNOSTIC_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<PreviewWindow*>(GetWindowLongPtr(hWnd, AGNOSTIC_USERDATA));
	}

	if (!pThis)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_DESTROY:
		pThis->WmDestroy();
	break;
	case WM_CREATE:
		pThis->WmCreate();
		break;
	case WM_PAINT:
		pThis->WmPaint(hWnd, message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT PreviewWindow::WmCreate()
{
	NONCLIENTMETRICS hfDefault;
	hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0);
	hFontStd = CreateFontIndirectW(&hfDefault.lfCaptionFont);

	// This should use the same text measurement that
	// we will ultimately use when rendering final text.
	LPRECT textDims = new RECT({ 0, 0,  ScaleX(800), ScaleY(100) });
	MeasureStringMultilineWrap(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", hFontStd, textDims);
	textDrawHeight = textDims->bottom;
	delete textDims;
	return 0;
}

LRESULT PreviewWindow::WmDestroy()
{
	DeleteObject(hFontStd);
	return 0;
}

LRESULT PreviewWindow::WmPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (previewClip == nullptr)
	{
		return 0;
	}

	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hWnd, &ps);
	HPEN hpenInitial = static_cast<HPEN>(SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(180, 180, 180))));

	Rectangle(hdc, previewRect->left + ScaleX(6), previewRect->top + ScaleY(6), previewRect->right + ScaleX(20), previewRect->bottom + ScaleY(18));

	LPRECT textRect = new RECT();
	CopyRect(textRect, previewRect);
	textRect->top += ScaleY(10);
	textRect->left += ScaleX(12);
	textRect->right += ScaleX(12);

	SelectObject(hdc, hFontStd);

	if (remainingTextLines > 0 || clipSizeInKb > MAX_LEN_PRV_CLIP_KB)
	{
		textRect->bottom += ScaleY(10) - textDrawHeight;

		LPRECT infoBreakRect = new RECT();
		CopyRect(infoBreakRect, previewRect);
		infoBreakRect->top = infoBreakRect->bottom - textDrawHeight + ScaleY(16);
		infoBreakRect->left += ScaleX(6);
		infoBreakRect->right += ScaleX(20);
		infoBreakRect->bottom += ScaleY(18);

		HBRUSH hbrushInitial = static_cast<HBRUSH>(SelectObject(hdc, CreateSolidBrush(RGB(240, 240, 245))));

		Rectangle(hdc, infoBreakRect->left, infoBreakRect->top, infoBreakRect->right, infoBreakRect->bottom);

		SetTextColor(hdc, RGB(100, 100, 120));

		if (remainingTextLines > 0)
		{
			DrawText(hdc, ((L"+ " + std::to_wstring(remainingTextLines) + L" more lines / " + std::to_wstring(clipSizeInKb) + L"KB total").c_str()), -1, infoBreakRect, DT_WORDBREAK | DT_EXTERNALLEADING | DT_NOPREFIX | DT_EDITCONTROL | DT_CENTER | DT_VCENTER);
		}
		else
		{
			DrawText(hdc, ((std::to_wstring(clipSizeInKb) + L"KB total").c_str()), -1, infoBreakRect, DT_WORDBREAK | DT_EXTERNALLEADING | DT_NOPREFIX | DT_EDITCONTROL | DT_CENTER | DT_VCENTER);
		}

		SetTextColor(hdc, BLACK_BRUSH);
		SelectObject(hdc, hbrushInitial);
		delete infoBreakRect;
	}
	else
	{
		textRect->bottom += ScaleY(10);
	}

	SelectObject(hdc, hpenInitial);
	DrawText(hdc, previewClip, -1, textRect, DT_WORDBREAK | DT_EXTERNALLEADING | DT_NOPREFIX | DT_EDITCONTROL);

	EndPaint(hWnd, &ps);
	delete textRect;
	return 0;
}

PreviewWindow::PreviewWindow(HINSTANCE hInstance)
{
	LoadStringW(hInstance, IDC_WINCPREVIEW, szPreviewWindowClass, MAX_LOADSTRING);

	RegisterPreviewWindowClass(hInstance);
}

PreviewWindow::~PreviewWindow()
{
}

bool PreviewWindow::InitPreviewWindow(HINSTANCE hInstance, HWND parentWnd)
{
	HWND hWnd = CreateWindowExW(0, szPreviewWindowClass, L"", WS_POPUP | WS_EX_TRANSPARENT,
		0, 0, ScaleX(400), ScaleY(400), parentWnd, nullptr, hInstance, this);

	if (!hWnd)
	{
		return false;
	}


	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	windowHandle = hWnd;

	return true;
}

HWND PreviewWindow::GetHandle()
{
	return windowHandle;
}

void PreviewWindow::SetPreviewClip(wchar_t * clip)
{
	previewClip = clip;
}

void PreviewWindow::MoveRelativeToRect(LPRECT rect)
{
	// Let's let our max size be 500
	long width = ScaleX(500);
	long height = ScaleY(500);

	LPRECT previewSize = new RECT({ 0, 0, width, height });

	clipSizeInKb = (wcslen(previewClip) * sizeof(wchar_t)) / 1024;;

	if (clipSizeInKb > MAX_LEN_PRV_CLIP_KB)
	{
		remainingTextLines = 0;
	}
	else
	{
		MeasureStringMultilineWrap(previewClip, hFontStd, previewSize);

		if (previewSize->right < width)
		{
			width = previewSize->right;
		}
		if (previewSize->bottom < height)
		{
			remainingTextLines = 0;
			height = previewSize->bottom;
		}
		else
		{
			// Get an approximation of the remaining lines to display.
			remainingTextLines = ((previewSize->bottom - height) / textDrawHeight);

			previewSize->bottom = height;
		}
	}

	CopyRect(previewRect, previewSize);
	delete previewSize;

	int xLoc = rect->right + ScaleX(5);
	int yLoc = rect->top;

	LPMONITORINFO lpmi = new MONITORINFO;
	lpmi->cbSize = sizeof(MONITORINFO);
	HMONITOR currentMonitor = MonitorFromRect(rect, 0);

	GetMonitorInfo(currentMonitor, lpmi);

	// These two if's account for the preview going off screen.
	// their position is precalculated, and if they would move out
	// of the desktop area, they are recalculated to appear on
	// the other side of the popupmenu, unless this would make
	// them go off screen.
	if (xLoc + width + ScaleX(32) > (lpmi->rcWork).right)
	{
		int xLocTemp = rect->left - (width + ScaleX(32));
		if (xLocTemp > (lpmi->rcMonitor).left)
		{
			xLoc = xLocTemp;
		}
	}
	if (yLoc + height + ScaleX(24) > (lpmi->rcWork).bottom)
	{
		int yLocTemp = rect->bottom - (height + ScaleY(24));
		if (yLocTemp > (lpmi->rcMonitor).top)
		{
			yLoc = yLocTemp;
		}
	}

	delete lpmi;

	// The two flags at the end ensure that we don't loose focus of the popup menu.
	RedrawWindow(GetHandle(), NULL, NULL, RDW_INVALIDATE);
	SetWindowPos(GetHandle(), HWND_TOPMOST, xLoc, yLoc, width + ScaleX(26), height + ScaleY(24), SWP_NOACTIVATE | SWP_NOSENDCHANGING);
}

void PreviewWindow::Show()
{
	ShowWindow(GetHandle(), SW_SHOWNA);
}

void PreviewWindow::Hide()
{
	ShowWindow(GetHandle(), SW_HIDE);
}

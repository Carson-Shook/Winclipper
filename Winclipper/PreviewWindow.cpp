#include "stdafx.h"
#include "PreviewWindow.h"

ATOM PreviewWindow::RegisterPreviewWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

	wcex.style = CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PreviewWindow::PreviewWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szPreviewWindowClass;

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK PreviewWindow::PreviewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// We want to capture a pointer to the current instance of the class
	// so we can call non-static methods from a static context.
	PreviewWindow * pThis = nullptr;

	if (message == WM_NCCREATE)
	{
#pragma warning( suppress : 26490 )
		pThis = static_cast<PreviewWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
#pragma warning( suppress : 26490 )
		if (!SetWindowLongPtrW(hWnd, AGNOSTIC_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
		return TRUE;
	}
	else
	{
#pragma warning( suppress : 26490 )
		pThis = reinterpret_cast<PreviewWindow*>(GetWindowLongPtrW(hWnd, AGNOSTIC_USERDATA));
	}

	if (!pThis)
	{
		return DefWindowProcW(hWnd, message, wParam, lParam);
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
		{
			if (SUCCEEDED(pThis->WmPaint(hWnd, message, wParam, lParam)))
			{
				ValidateRect(hWnd, nullptr);
			}
		}
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT PreviewWindow::WmCreate()
{
	return 0;
}

LRESULT PreviewWindow::WmDestroy()
{
	return 0;
}

LRESULT PreviewWindow::WmPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;

	hr = CreateDeviceDependentResources(hWnd);

	std::wstring infoBreakText;

	if (remainingTextLines > 0)
	{
		infoBreakText = L"+ " + std::to_wstring(remainingTextLines) + L" more lines / " + std::to_wstring(clipSizeInKb) + L"KB total";
	}
	else if (clipSizeInKb > MAX_LEN_PRV_CLIP_KB)
	{
		infoBreakText = std::to_wstring(clipSizeInKb) + L"KB total";
	}

	IDWriteTextLayout * pDWriteInfoBreakLayout = nullptr;
	if (SUCCEEDED(hr))
	{
		hr = pDWriteFactory->CreateTextLayout(
			infoBreakText.c_str(),
			static_cast<UINT32>(infoBreakText.length()),
			pDWriteInfoBreakFormat,
			pRT->GetSize().width - (windowBorderWidth * 2.0f),
			infoBreakHeight,
			&pDWriteInfoBreakLayout
		);
	}
	
	if (SUCCEEDED(hr))
	{
		pDWriteInfoBreakLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		pRT->BeginDraw();

		pRT->SetTransform(D2D1::Matrix3x2F::Identity());

		pRT->Clear(WindowColor);

		pRT->FillRectangle(
			D2D1::RectF(
				windowBorderWidth,
				windowBorderWidth,
				pRT->GetSize().width - windowBorderWidth,
				pRT->GetSize().height - windowBorderWidth),
			pWhiteBrush);

		pRT->DrawRectangle(
			D2D1::RectF(
				windowBorderWidth,
				windowBorderWidth,
				pRT->GetSize().width - windowBorderWidth,
				pRT->GetSize().height - windowBorderWidth),
			pLightGrayBrush, 0.5F);

		pRT->DrawTextLayout(
			D2D1::Point2F(
				windowBorderWidth + textMarginWidth,
				windowBorderWidth + textMarginHeight - 1.0f), // 1 unit nudge up because it looks like it's off by 1 otherwise.
			pDWriteTextLayout,
			pBlackBrush,
			textDrawOptions);

		if (remainingTextLines > 0 || clipSizeInKb > MAX_LEN_PRV_CLIP_KB)
		{
			pRT->FillRectangle(
				D2D1::RectF(
					windowBorderWidth,
					pRT->GetSize().height - infoBreakHeight - windowBorderWidth,
					pRT->GetSize().width - windowBorderWidth,
					pRT->GetSize().height - windowBorderWidth),
				pBlueGrayBrush);

			pRT->DrawRectangle(
				D2D1::RectF(
					windowBorderWidth,
					pRT->GetSize().height - infoBreakHeight - windowBorderWidth,
					pRT->GetSize().width - windowBorderWidth,
					pRT->GetSize().height - windowBorderWidth),
				pLightGrayBrush, 0.5F);

			pRT->DrawTextLayout(
				D2D1::Point2F(
					windowBorderWidth,
					pRT->GetSize().height - infoBreakHeight - windowBorderWidth),
				pDWriteInfoBreakLayout,
				pLabelTextBrush,
				D2D1_DRAW_TEXT_OPTIONS_NONE);
		}

		hr = pRT->EndDraw();
	}
	SafeRelease(&pDWriteInfoBreakLayout);
	//SafeRelease(&pDWriteTextLayout);

	if (hr == D2DERR_RECREATE_TARGET)
	{
		DestroyDeviceDependentResources();
	}

	return hr;
}

HRESULT PreviewWindow::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;
	
	if (IsWindows8Point1OrGreater())
	{
		textDrawOptions = D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | D2D1_DRAW_TEXT_OPTIONS_CLIP;
	}
	else
	{
		textDrawOptions = D2D1_DRAW_TEXT_OPTIONS_CLIP;
	}

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&pD2DFactory
	);

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pDWriteFactory),
			reinterpret_cast<IUnknown **>(&pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr)) // quick escape if factory fails.
	{
		/******** This section gets the system font ********/

		IDWriteGdiInterop * pGdiInterop = nullptr;
		IDWriteFont * pDWriteSysFont = nullptr;
		IDWriteFontFamily * pFontFamily = nullptr;
		IDWriteLocalizedStrings* pFontFamilyName = nullptr;
		NONCLIENTMETRICS hfDefault = NONCLIENTMETRICS();

		if (SUCCEEDED(hr))
		{
			hr = pDWriteFactory->GetGdiInterop(&pGdiInterop);
		}

		if (SUCCEEDED(hr))
		{
			hfDefault.cbSize = sizeof(NONCLIENTMETRICS);
			if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &hfDefault, 0))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			else
			{ 
				hr = pGdiInterop->CreateFontFromLOGFONT(&hfDefault.lfMenuFont, &pDWriteSysFont);
			}
		}

		if (SUCCEEDED(hr))
		{
			hr = pDWriteSysFont->GetFontFamily(&pFontFamily);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFontFamily->GetFamilyNames(&pFontFamilyName);
		}

		UINT32 index = 0;
		UINT32 length = 0;
		BOOL exists = FALSE;

		wchar_t locale[LOCALE_NAME_MAX_LENGTH];
		if (!GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH))
		{
			// If we can't get the locale, default to en-us
			wcsncpy_s(locale, L"en-us", LOCALE_NAME_MAX_LENGTH);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFontFamilyName->FindLocaleName(locale, &index, &exists);
		}

		if (SUCCEEDED(hr) && !exists) // if the above find did not find a match, retry with US English
		{
			hr = pFontFamilyName->FindLocaleName(L"en-us", &index, &exists);
		}

		if (!exists)
		{
			index = 0;
		}

		if (SUCCEEDED(hr))
		{
			hr = pFontFamilyName->GetStringLength(index, &length);
		}

		// The array that will eventually contain our font fontName.
		wchar_t * fontName = new (std::nothrow) wchar_t[long long{ length } +1];
		if (fontName == nullptr)
		{
			hr = E_OUTOFMEMORY;
		}

		if (SUCCEEDED(hr))
		{
			hr = pFontFamilyName->GetString(index, fontName, length + 1);
		}
		/******** Crazy section over ********/

		// ClearType causes a normal font weight that
		// isn't pixel perfect to look like garbage.
		// A medium font looks closer to a normal font
		// on a high-dpi display in this case.
		// Results may vary based on ClearType tuning.
		if (ScaleX(1) == 1)
		{
			weight = DWRITE_FONT_WEIGHT_MEDIUM;
		}
		else
		{
			weight = DWRITE_FONT_WEIGHT_NORMAL;
		}

		if (SUCCEEDED(hr))
		{
			hr = pDWriteFactory->CreateTextFormat(
				fontName,
				nullptr,
				weight,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				12.0f,
				locale,
				&pDWriteTextFormat
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDWriteFactory->CreateTextFormat(
				fontName,
				nullptr,
				weight,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				12.0f,
				locale,
				&pDWriteInfoBreakFormat
			);
		}

		delete[] fontName;
	}

	return hr;
}

HRESULT PreviewWindow::CreateDeviceDependentResources(HWND hWnd) noexcept
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);

	if (pRT == nullptr)
	{
		hr = pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				D2D1::SizeU(
					rc.right - rc.left,
					rc.bottom - rc.top),
				D2D1_PRESENT_OPTIONS_IMMEDIATELY
			),
			&pRT
		);
		if (SUCCEEDED(hr))
		{
			pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
		}
	}
	else
	{
		hr = pRT->Resize(
			D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top)
		);
	}

	if (SUCCEEDED(hr) && pLightGrayBrush == nullptr)
	{
		pRT->CreateSolidColorBrush(
			D2D1::ColorF(0xB4B4B4),
			&pLightGrayBrush
		);
	}

	if (SUCCEEDED(hr) && pWhiteBrush == nullptr)
	{
		pRT->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&pWhiteBrush
		);
	}

	if (SUCCEEDED(hr) && pBlackBrush == nullptr)
	{
		pRT->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&pBlackBrush
		);
	}

	if (SUCCEEDED(hr) && pLabelTextBrush == nullptr)
	{
		pRT->CreateSolidColorBrush(
			D2D1::ColorF(0x646478),
			&pLabelTextBrush
		);
	}

	if (SUCCEEDED(hr) && pBlueGrayBrush == nullptr)
	{
		pRT->CreateSolidColorBrush(
			D2D1::ColorF(0xF0F0F5),
			&pBlueGrayBrush
		);
	}

	return hr;
}

void PreviewWindow::DestroyDeviceIndependentResources()
{
	SafeRelease(&pDWriteInfoBreakFormat);
	SafeRelease(&pDWriteTextFormat);
	SafeRelease(&pD2DFactory);
	return;
}

void PreviewWindow::DestroyDeviceDependentResources()
{
	SafeRelease(&pBlueGrayBrush);
	SafeRelease(&pLightGrayBrush);
	SafeRelease(&pWhiteBrush);
	SafeRelease(&pBlackBrush);
	SafeRelease(&pLabelTextBrush);
	SafeRelease(&pRT);
	return;
}

PreviewWindow::PreviewWindow(HINSTANCE hInstance, HWND hWndParent)
{
	LoadStringW(hInstance, IDC_WINCPREVIEW, szPreviewWindowClass, MAX_LOADSTRING);

	CreateDeviceIndependentResources();

	RegisterPreviewWindowClass(hInstance);

	InitPreviewWindow(hInstance, hWndParent);
}

PreviewWindow::~PreviewWindow()
{
	DestroyDeviceDependentResources();
	DestroyDeviceIndependentResources();
}

bool PreviewWindow::InitPreviewWindow(HINSTANCE hInstance, HWND hWndParent)
{
	HWND hWnd = CreateWindowExW(0, szPreviewWindowClass, L"Winclipper Preview Window", WS_POPUP,
		0, 0, ScaleX(400), ScaleY(400), hWndParent, nullptr, hInstance, this);

	if (!hWnd)
	{
		return false;
	}

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	windowHandle = hWnd;

	return true;
}

HWND PreviewWindow::GetHandle() noexcept
{
	return windowHandle;
}

void PreviewWindow::SetPreviewClip(wchar_t * clip) noexcept
{
	previewClip = clip;
}

void PreviewWindow::MoveRelativeToRect(const LPRECT rect, unsigned int index)
{
	if (previewClip == nullptr)
	{
		return;
	}

	if (pDWriteTextFormat == nullptr)
	{
		DestroyDeviceDependentResources();
		return;
	}

	HRESULT hr = S_OK;

	// this will make more sense once max preview size is configurable;
	const float layoutMaxWidth = windowMaxWidth - (windowBorderWidth * 2.0f) - (textMarginWidth * 2.0f);
	const float layoutMaxHeight = windowMaxHeight - (windowBorderWidth * 2.0f) - (textMarginHeight * 2.0f);

	float renderingWidth = 0.0f;
	float renderingHeight = 0.0f;

	const size_t clipLength = wcslen(previewClip);
	
	clipSizeInKb = clipLength / 1024;

	if (layoutCache.find(index) != layoutCache.end())
	{
		pDWriteTextLayout = layoutCache.at(index);
	}
	else
	{
		if (clipSizeInKb > MAX_LEN_PRV_CLIP_KB)
		{
			std::wstring fallbackMessage = L"Clip too large to preview.\r\n\t\t" + std::to_wstring(clipSizeInKb) + L"KB Total";

			hr = pDWriteFactory->CreateTextLayout(
				fallbackMessage.c_str(),
				static_cast<UINT32>(fallbackMessage.length()),
				pDWriteTextFormat,
				layoutMaxWidth,
				layoutMaxHeight,
				&pDWriteTextLayout
			);
		}
		else
		{
			hr = pDWriteFactory->CreateTextLayout(
				previewClip,
				static_cast<UINT32>(wcslen(previewClip)),
				pDWriteTextFormat,
				layoutMaxWidth,
				layoutMaxHeight,
				&pDWriteTextLayout
			);
		}

		if (SUCCEEDED(hr))
		{
			layoutCache.insert_or_assign(index, pDWriteTextLayout);
		}
	}
	if (SUCCEEDED(hr))
	{
		pDWriteTextLayout->SetIncrementalTabStop(18.0f);
	}

	if (SUCCEEDED(hr))
	{
		pDWriteTextLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 16.0f, 12.0f);
	}

	DWRITE_TEXT_METRICS textMetrics = DWRITE_TEXT_METRICS();

	if (SUCCEEDED(hr))
	{
		hr = pDWriteTextLayout->GetMetrics(&textMetrics);
	}

	if (SUCCEEDED(hr))
	{
		renderingWidth = textMetrics.widthIncludingTrailingWhitespace;
		renderingHeight = textMetrics.height;

		if (renderingWidth > layoutMaxWidth)
		{
			renderingWidth = layoutMaxWidth;
		}

		if (renderingHeight > layoutMaxHeight)
		{
			// Calculate the non-visible lines remaining. The 1 accounts for the line we chop off.
			remainingTextLines = static_cast<unsigned long long>((static_cast<double>(textMetrics.lineCount) * ((renderingHeight - layoutMaxHeight) / renderingHeight))) + 1;
			renderingHeight = layoutMaxHeight;
			pDWriteTextLayout->SetMaxHeight(layoutMaxHeight - 16);

			// make sure that remaining line information can display
			if (renderingWidth < 180)
			{
				renderingWidth = 180;
			}
		}
		else
		{
			clipSizeInKb = 0;
			remainingTextLines = 0;
		}
	}

	const int totalWindowWidth = static_cast<int>(ScaleX(renderingWidth + (windowBorderWidth * 2.0f) + (textMarginWidth * 2.0f)));
	const int totalWindowHeight = static_cast<int>(ScaleY(renderingHeight + (windowBorderWidth * 2.0f) + (textMarginHeight * 2.0f)));

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
	// 2018-09-09: width is now based on the maximum window size.
	// I was tired of it flipping back and forth constantly.
	if (xLoc + ScaleX(windowMaxWidth) + ScaleX(8) > (lpmi->rcWork).right)
	{
		const int xLocTemp = rect->left - (totalWindowWidth + ScaleX(8));
		if (xLocTemp > (lpmi->rcWork).left)
		{
			xLoc = xLocTemp;
		}
	}
	if (yLoc + totalWindowHeight + ScaleX(24) > (lpmi->rcWork).bottom)
	{
		const int yLocTemp = rect->bottom - (totalWindowHeight);
		if (yLocTemp > (lpmi->rcWork).top)
		{
			yLoc = yLocTemp;
		}
	}

	delete lpmi;

	// The two flags at the end ensure that we don't loose focus of the popup menu.
	SetWindowPos(GetHandle(), HWND_TOPMOST, xLoc, yLoc, totalWindowWidth, totalWindowHeight, SWP_NOACTIVATE | SWP_NOSENDCHANGING);
	RedrawWindow(GetHandle(), nullptr, nullptr, RDW_INVALIDATE);
}

void PreviewWindow::Show() noexcept
{
	ShowWindow(GetHandle(), SW_SHOWNA);
}

void PreviewWindow::Hide() noexcept
{
	ShowWindow(GetHandle(), SW_HIDE);
}

void PreviewWindow::ClearCache()
{
	// this will get more fleshed out in the future
	try
	{
		if (!layoutCache.empty())
		{
			for (auto pair : layoutCache)
			{
				SafeRelease(&(pair.second));
			}
			layoutCache.clear();
		}
	}
	catch(const std::exception e) 
	{
		// outside of unexpected behavior,
		// I don't anticipate any exceptions
		// but if they do happen, let's 
		// just let the memory leak and
		// clear the cache rather than crash.
		layoutCache.clear();
	}
}
#pragma once
#include "ControlUtilities.h"
#include "resource.h"
#include "string.h"
#include <VersionHelpers.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <d3d11.h>
#include "ControlUtilities.h"

#define MAX_LEN_PRV_CLIP_KB		30

class PreviewWindow
{
private:
	const float					windowBorderWidth = 6.5f;
	const float					windowMaxWidth = 500.0f;
	const float					windowMaxHeight = 500.0f;
	const float					textMarginWidth = 5.5f;
	const float					textMarginHeight = 3.5f;
	const float					infoBreakHeight = 18.0f;

	ID2D1Factory *				pD2DFactory = NULL;
	IDWriteFactory *			pDWriteFactory = NULL;
	ID2D1HwndRenderTarget *		pRT = NULL;
	IDWriteTextFormat *			pDWriteTextFormat = NULL;
	IDWriteTextFormat *			pDWriteInfoBreakFormat = NULL;
	IDWriteRenderingParams *	pDWriteRenderingParams = NULL;
	D2D1::ColorF				WindowColor = D2D1::ColorF(0xF0F0F5);

	ID2D1SolidColorBrush *		pLightGrayBrush = NULL;
	ID2D1SolidColorBrush *		pWhiteBrush = NULL;
	ID2D1SolidColorBrush *		pBlackBrush = NULL;
	ID2D1SolidColorBrush *		pLabelTextBrush = NULL;
	ID2D1SolidColorBrush *		pBlueGrayBrush = NULL;
	D2D1_DRAW_TEXT_OPTIONS		textDrawOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;

	IDWriteTextLayout *			pDWriteTextLayout = NULL;
	DWRITE_FONT_WEIGHT			weight;

	wchar_t *					previewClip = nullptr;
	unsigned long long			remainingTextLines = 0; // need to account for theoretical maximum NTFS file size
	unsigned long long			clipSizeInKb = 0;
	wchar_t						szPreviewWindowClass[MAX_LOADSTRING];       // the text preview window
	LPRECT						previewRect = new RECT({ 0, 0, ScaleX(500), ScaleY(500) });
	HFONT						hFontStd;
	HWND						windowHandle;

	ATOM						RegisterPreviewWindowClass(HINSTANCE);
	static LRESULT CALLBACK		PreviewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCreate();
	LRESULT						WmDestroy();
	LRESULT						WmPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT						CreateDeviceIndependentResources();
	HRESULT						CreateDeviceDependentResources(HWND hWnd);
	void						DestroyDeviceIndependentResources();
	void						DestroyDeviceDependentResources();

	template <class T> inline void		SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

public:
								PreviewWindow(HINSTANCE hInstance, HWND hWndParent);
								~PreviewWindow();

	bool						InitPreviewWindow(HINSTANCE hInstance, HWND parentWnd);

	HWND						GetHandle();
	void						SetPreviewClip(wchar_t * clip);
	void						MoveRelativeToRect(LPRECT rect);
	void						Show();
	void						Hide();
};


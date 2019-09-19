#pragma once
#include "ControlUtilities.h"
#include "resource.h"
#include "string.h"
#include <map>
#include <VersionHelpers.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <d3d11.h>
#include "ControlUtilities.h"
#include "Clip.h"

#define MAX_LEN_PRV_CLIP_KB		60

class PreviewWindow
{
private:
	const float					windowBorderWidth = 6.5f;
	const float					windowMaxWidth = 500.0f;
	const float					windowMaxHeight = 500.0f;
	const float					textMarginWidth = 5.5f;
	const float					textMarginHeight = 3.5f;
	const float					infoBreakHeight = 18.0f;

	IDWriteFactory *			pDWriteFactory = nullptr;
	IDWriteTextFormat *			pDWriteTextFormat = nullptr;
	IDWriteTextFormat *			pDWriteInfoBreakFormat = nullptr;
	IDWriteTextLayout *			pDWriteTextLayout = nullptr;
	IDWriteRenderingParams *	pDWriteRenderingParams = nullptr;

	IWICImagingFactory *		pIWICFactory = nullptr;
	IWICFormatConverter *		pConvertedSourceBitmap = nullptr;
	IWICFormatConverter *		pConvertedLoadingBitmap = nullptr;

	ID2D1Factory *				pD2DFactory = nullptr;
	ID2D1HwndRenderTarget *		pRT = nullptr;
	ID2D1Bitmap *				pD2DBitmap = nullptr;
	ID2D1Bitmap *				pD2DLoadingBitmap = nullptr;
	ID2D1SolidColorBrush *		pLightGrayBrush = nullptr;
	ID2D1SolidColorBrush *		pWhiteBrush = nullptr;
	ID2D1SolidColorBrush *		pBlackBrush = nullptr;
	ID2D1SolidColorBrush *		pLabelTextBrush = nullptr;
	ID2D1SolidColorBrush *		pBlueGrayBrush = nullptr;
	
	D2D1_BITMAP_INTERPOLATION_MODE	interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
	D2D1_DRAW_TEXT_OPTIONS		textDrawOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;
	DWRITE_FONT_WEIGHT			weight = DWRITE_FONT_WEIGHT_NORMAL;
	D2D1::ColorF				WindowColor = D2D1::ColorF(0xF0F0F5);

	std::map<unsigned int, IDWriteTextLayout *> layoutCache;

	unsigned int				timerAttempts = 0;
	bool						clipChanged = false;

	std::shared_ptr<Clip>		previewClip = nullptr;
	unsigned long long			remainingTextLines = 0; // need to account for theoretical maximum NTFS file size
	unsigned long long			clipSizeInKb = 0;
	wchar_t						szPreviewWindowClass[MAX_LOADSTRING];       // the text preview window
	HWND						windowHandle = nullptr;
	bool						bitmapReady = false;

	ATOM						RegisterPreviewWindowClass(HINSTANCE);
	static LRESULT CALLBACK		PreviewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT						WmCreate();
	LRESULT						WmDestroy();
	LRESULT						WmTimer();
	LRESULT						WmPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT						CreateDeviceIndependentResources(HINSTANCE hInstance);
	HRESULT						CreateDeviceDependentResources(HWND hWnd) noexcept;
	void						DestroyDeviceIndependentResources();
	void						DestroyDeviceDependentResources();
	HRESULT						SetBitmapConverter();

public:
	PreviewWindow(HINSTANCE hInstance, HWND hWndParent);
	~PreviewWindow();

	bool						InitPreviewWindow(HINSTANCE hInstance, HWND parentWnd);
	HWND						GetHandle() noexcept;
	void						SetPreviewClip(std::shared_ptr<Clip> clip) noexcept;
	void						MoveRelativeToRect(LPRECT rect, unsigned int index);
	void						Show() noexcept;
	void						Hide() noexcept;
	void						ClearCache();
};


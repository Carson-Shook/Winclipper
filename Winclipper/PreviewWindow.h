#pragma once
#include "ControlUtilities.h"
#include "resource.h"
#include "string.h"
#include "ControlUtilities.h"

#define MAX_LEN_PRV_CLIP_KB		80

class PreviewWindow
{
private:
	wchar_t *					previewClip = nullptr;
	long						textDrawHeight;
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

public:
								PreviewWindow(HINSTANCE hInstance);
								~PreviewWindow();

	bool						InitPreviewWindow(HINSTANCE hInstance, HWND parentWnd);

	HWND						GetHandle();
	void						SetPreviewClip(wchar_t * clip);
	void						MoveRelativeToRect(LPRECT rect);
	void						Show();
	void						Hide();
};


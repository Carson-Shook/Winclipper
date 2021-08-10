#pragma once
#include "framework.h"
#include "Font.h"

namespace Controls
{
	class Control
	{
	private:
		bool layoutSuspended = false;

	protected:
		HINSTANCE hInstance = nullptr;
		HWND parentHandle = nullptr;
		HWND handle = nullptr;
		int dpiX = 96;
		int dpiY = 96;
		UINT_PTR id = 0;
		HFONT font = Controls::Font::SystemDefault;
		bool exists = false;

		int InitDpiX();
		int InitDpiY();
		int DpiScaleX(int x) noexcept;
		int DpiScaleY(int y) noexcept;
		void MeasureString(LPCWSTR text, HFONT font, LPRECT rect);

		virtual bool Create();

	public:
		Control();
		Control(HWND hWnd, UINT_PTR id, HINSTANCE hInstance);
		
		void PerformLayout();
		void SuspendLayout();
		void ResumeLayout(bool layoutImmediately = true);

		int X = 0;
		int Y = 0;
		int Width = 10;
		int Height = 10;
		LPCWSTR Text = nullptr;

		HWND Handle();
		HWND ParentHandle();
		bool Exists();
		bool LayoutSuspended();

		virtual void SetEnabled(bool enabled);
		void SetText(LPCWSTR text);
		void SetFont(HFONT font);
	};
}
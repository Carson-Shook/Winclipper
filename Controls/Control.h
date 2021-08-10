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
		int x = 0;
		int y = 0;
		int width = 10;
		int height = 10;
		bool enabled = true;
		LPCWSTR text = nullptr;
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
		
		virtual void PerformCustomLayout();
		void PerformLayout();
		void SuspendLayout();
		void ResumeLayout(bool layoutImmediately = true);
		bool LayoutSuspended();

		HWND Handle();
		HWND ParentHandle();
		bool Exists();

		virtual void SetPosition(int x, int y, int height, int width);

		virtual int X();
		virtual void SetX(int x);
		
		virtual int Y();
		virtual void SetY(int y);
		
		virtual int Width();
		virtual void SetWidth(int width);
		
		virtual int Height();
		virtual void SetHeight(int height);

		virtual bool Enabled();
		virtual void SetEnabled(bool enabled);
		
		virtual LPCWSTR Text();
		virtual void SetText(LPCWSTR text);
		
		virtual HFONT Font();
		virtual void SetFont(HFONT font);
	};
}
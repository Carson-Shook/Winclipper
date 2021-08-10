#pragma once
#include "Control.h"

namespace Controls
{
	class UpDown : public Control
	{
	protected:
		int minValue = 0;
		int maxValue = 999;
		
		bool Create() override;
	public:
		UpDown();
		UpDown(HWND hWnd, UINT_PTR id, HINSTANCE hInstance);

		void PerformCustomLayout() override;

		virtual int MinValue();
		virtual void SetMinValue(int value);
		
		virtual int MaxValue();
		virtual void SetMaxValue(int value);
	};
}
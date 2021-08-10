#pragma once
#include "Control.h"
#include "Edit.h"
#include "UpDown.h"

namespace Controls
{
	class Spinner : public Control
	{
	protected:
		UINT_PTR upDownId;
		Edit upDownBuddyInternal;
		UpDown upDownInternal;

		int minValue = 0;
		int maxValue = 999;

		bool Create() override;
	public:
		Spinner();
		Spinner(HWND hWnd, UINT_PTR editId, UINT_PTR upDownId, HINSTANCE hInstance);
		
		void PerformCustomLayout() override;

		bool Enabled() override;
		void SetEnabled(bool enabled) override;

		virtual int MinValue();
		virtual void SetMinValue(int value);

		virtual int MaxValue();
		virtual void SetMaxValue(int value);
	};
}
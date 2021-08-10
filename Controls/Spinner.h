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
		
		bool Create() override;
	public:
		int MinValue = 0;
		int MaxValue = 999;

		Spinner();
		Spinner(HWND hWnd, UINT_PTR editId, UINT_PTR upDownId, HINSTANCE hInstance);
		void SetEnabled(bool enabled) override;
	};
}
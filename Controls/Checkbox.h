#pragma once
#include "Control.h"
#include "Label.h"

namespace Controls
{
	class Checkbox : public Control
	{
	private:
		bool valueInternal;
	protected:
		bool Create() override;
	public:
		bool AutoSize = true;
		bool UseBasicTheme = false;
		
		bool Value();
		void SetValue(bool value);

		Checkbox();
		Checkbox(HWND hWnd, UINT_PTR id, HINSTANCE hInstance);
		int GetCheckboxWidth();
		int GetCheckboxBorderWidth();
	};
}
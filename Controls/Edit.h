#pragma once
#include "Control.h"

namespace Controls
{
	class Edit : public Control
	{
	protected:
		bool Create() override;
	public:
		bool ReadOnly = false;
		bool Multiline = false;
		bool VerticalScrolling = false;
		bool HorizontalScrolling = false;
		bool Numeric = false;

		Edit();
		Edit(HWND hWnd, UINT_PTR id, HINSTANCE hInstance);
	};
}
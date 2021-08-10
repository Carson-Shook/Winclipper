#pragma once
#include "Control.h"

namespace Controls
{
	class Label : public Control
	{
	protected:
		bool Create() override;
	public:
		bool AutoSize = true;
		
		Label();
		Label(HWND hWnd, UINT_PTR id, HINSTANCE hInstance);
	};
}
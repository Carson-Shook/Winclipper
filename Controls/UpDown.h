#pragma once
#include "Control.h"

namespace Controls
{
	class UpDown : public Control
	{
	protected:
		bool Create() override;
	public:
		int MinValue = 0;
		int MaxValue = 999;

		UpDown();
		UpDown(HWND hWnd, UINT_PTR id, HINSTANCE hInstance);

	};
}
#include "Spinner.h"

bool Controls::Spinner::Create()
{
	upDownBuddyInternal = Edit(parentHandle, id, hInstance);
	upDownBuddyInternal.SuspendLayout();
	upDownBuddyInternal.SetX(x);
	upDownBuddyInternal.SetY(y);
	upDownBuddyInternal.SetWidth(width);
	upDownBuddyInternal.SetHeight(height);
	upDownBuddyInternal.Numeric = true;
	upDownBuddyInternal.ResumeLayout();
	
	upDownInternal = UpDown(parentHandle, upDownId, hInstance);
	upDownInternal.SuspendLayout();
	upDownInternal.MinValue = MinValue;
	upDownInternal.MaxValue = MaxValue;
	upDownInternal.ResumeLayout();

	return upDownBuddyInternal.Exists() && upDownInternal.Exists();
}

Controls::Spinner::Spinner()
	: Control{}
{ }

Controls::Spinner::Spinner(HWND hWnd, UINT_PTR editId, UINT_PTR upDownId, HINSTANCE hInstance)
	: Control{ hWnd, editId, hInstance }
{ 
	Spinner::upDownId = upDownId;
	// Default size of a spinner
	width = 50;
	height = 19;
}

void Controls::Spinner::SetEnabled(bool enabled)
{
	upDownBuddyInternal.SetEnabled(enabled);
	upDownInternal.SetEnabled(enabled);
}
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
	upDownInternal.SetMinValue(minValue);
	upDownInternal.SetMaxValue(maxValue);
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

bool Controls::Spinner::Enabled()
{
	return upDownBuddyInternal.Enabled() && upDownInternal.Enabled();
}

void Controls::Spinner::SetEnabled(bool enabled)
{
	upDownBuddyInternal.SetEnabled(enabled);
	upDownInternal.SetEnabled(enabled);
}


void Controls::Spinner::PerformCustomLayout()
{
	SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(maxValue, minValue));    // Sets the controls direction 
}

int Controls::Spinner::MinValue()
{
	return minValue;
}

void Controls::Spinner::SetMinValue(int value)
{
	minValue = value;
	if (!LayoutSuspended())
	{
		SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(maxValue, minValue));
	}
}

int Controls::Spinner::MaxValue()
{
	return maxValue;
}

void Controls::Spinner::SetMaxValue(int value)
{
	maxValue = value;
	if (!LayoutSuspended())
	{
		SendMessageW(handle, UDM_SETRANGE, 0, MAKELPARAM(maxValue, minValue));
	}
}

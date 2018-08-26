#include "stdafx.h"
#include "Notify.h"

Notify::Notify()
{ 

}

Notify::~Notify()
{
	internalCollection.clear();
}

// Subscribes a given hWnd to any messages
// dispatched using NotifySubscribers().
// Returns true if successfully added to the
// internalCollection, or false otheriwse.
bool Notify::Subscribe(HWND hWnd)
{
	if (hWnd == NULL)
	{
		// shouldn't add a null hWnd
		return false;
	}

	// returns true if successfully inserted, false otherwise.
	return (internalCollection.insert(hWnd)).second;
}

// Removes a given hWnd from the subscriber
// internalCollection. Returns true if found
// and removed, or false if the value does not exist.
bool Notify::Unsubscribe(HWND hWnd)
{
	if (hWnd == NULL)
	{
		// don't attempt to remove a null hWnd
		return false;
	}

	return (internalCollection.erase(hWnd)) > 0;
}

// Posts a given WM_COMMAND message LOWORD to
// the message queue of each subscribed hWnd.
void Notify::SendNotifcation(WORD wmCommandLoword, HWND sender)
{
	if (!internalCollection.empty())
	{
		for (HWND hWnd : internalCollection)
		{
			if (sender == NULL)
			{
				SendMessage(hWnd, WM_COMMAND, wmCommandLoword, 0);
			}
			else
			{
				SendMessage(hWnd, WM_COMMAND, wmCommandLoword, (LPARAM)sender);
			}
		}
	}
}

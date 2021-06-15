#include "stdafx.h"
#include "Notify.h"

// Subscribes a given hWnd to any messages
// dispatched using NotifySubscribers().
// Returns true if successfully added to the
// internalCollection, or false otheriwse.
bool Notify::Subscribe(HWND hWnd)
{
	if (hWnd == nullptr)
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
	if (hWnd == nullptr)
	{
		// don't attempt to remove a null hWnd
		return false;
	}

	return (internalCollection.erase(hWnd)) > 0;
}

// Posts a given WM_COMMAND message LOWORD to
// the message queue of each subscribed hWnd.
void Notify::SendNotifcation(WORD wmCommandLoword, const HWND sender)
{
	if (!internalCollection.empty())
	{
		for (HWND hWnd : internalCollection)
		{
			if (sender == nullptr)
			{
				PostMessageW(hWnd, WM_COMMAND, wmCommandLoword, 0);
			}
			else
			{
#pragma warning( suppress : 26490 )
				PostMessageW(hWnd, WM_COMMAND, wmCommandLoword, reinterpret_cast<LPARAM>(sender));
			}
		}
	}
}

std::set<HWND> Notify::internalCollection;
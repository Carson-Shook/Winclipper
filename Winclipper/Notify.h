#pragma once
/*
 * Notify.h is an inheritable class that makes it
 * possible to subscribe to messages posted by the
 * class that inherits from it.
 * It should not generally be instantiated directly.
 */
#include "stdafx.h"
#include <set>

class Notify
{
private:
	static std::set<HWND> internalCollection; 
public:
	static bool Subscribe(HWND hWnd);
	static bool Unsubscribe(HWND hWnd);
	static void SendNotifcation(WORD wmCommnadLoword, const HWND sender);
};


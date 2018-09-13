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
	std::set<HWND> internalCollection; 
public:
	Notify() noexcept;
	~Notify();
	bool Subscribe(HWND hWnd);
	bool Unsubscribe(HWND hWnd);
	void SendNotifcation(WORD wmCommnadLoword, const HWND sender);
};


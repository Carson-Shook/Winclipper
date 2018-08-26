#pragma once
#include "stdafx.h"
#include <set>

class Notify
{
private:
	std::set<HWND> internalCollection; 
public:
	Notify();
	~Notify();
	bool Subscribe(HWND hWnd);
	bool Unsubscribe(HWND hWnd);
	void SendNotifcation(WORD wmCommnadLoword, HWND sender);
};


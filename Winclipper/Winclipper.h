#pragma once
#include "resource.h"
#include "string.h"
#include "UserSettings.h"
#include "ControlUtilities.h"
#include "settingsWindow.h"
#include "MainWindow.h"
#include "ClipsManager.h"

// Global Variables:
HINSTANCE hInst;                                    // current instance

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
bool					RunUpdater();

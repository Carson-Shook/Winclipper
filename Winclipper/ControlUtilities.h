#pragma once
#include "resource.h"

int ScaleX(int x) noexcept;
int ScaleY(int y) noexcept;
float ScaleX(float x) noexcept;
float ScaleY(float y) noexcept;

HWND AddHotkeyCtrl(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, UINT_PTR id);
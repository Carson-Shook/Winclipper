#pragma once

int ScaleX(int x);
int ScaleY(int y);

HWND AddLabel(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, int id);
HWND AddCheckbox(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, int id);
VOID AddSpinner(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, int max, int udId, int txtId);
HWND AddHotkeyCtrl(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, int id);
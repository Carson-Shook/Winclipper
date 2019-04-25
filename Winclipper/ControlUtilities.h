#pragma once
#include "resource.h"

int ScaleX(int x) noexcept;
int ScaleY(int y) noexcept;
float ScaleX(float x) noexcept;
float ScaleY(float y) noexcept;

void MeasureString(LPCWSTR text, HFONT font, LPRECT rect);

HWND AddLabel(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, UINT_PTR id);
HWND AddEdit(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, LPCWSTR text, UINT_PTR id, bool readOnly, bool multiLine, bool vScroll, bool hScroll);
HWND AddCheckbox(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, LPCWSTR text, UINT_PTR id);
VOID AddSpinner(HWND hWnd, HFONT font, int x, int y, HINSTANCE hIn, int min, int max, UINT_PTR udId, UINT_PTR txtId);
HWND AddHotkeyCtrl(HWND hWnd, HFONT font, int x, int y, int width, int height, HINSTANCE hIn, UINT_PTR id);
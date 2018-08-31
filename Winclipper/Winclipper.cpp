// Entry point for the application.
#include "stdafx.h"
#include "Winclipper.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	RunUpdater();

	UserSettings uSettings = UserSettings();

	SettingsWindow settingsWindow = SettingsWindow(hInstance, uSettings);
	MainWindow mainWindow = MainWindow(hInstance, uSettings, settingsWindow);

    hInst = hInstance; // Store instance handle in our global variable

	mainWindow.InitMainWindow(hInstance);
	settingsWindow.InitSettingsWindow(hInstance, nullptr);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINCSETTINGS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable,  &msg))
        {
            if (!IsDialogMessage(GetAncestor(msg.hwnd,GA_ROOTOWNER), &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return (int) msg.wParam;
}

// Execute StupidSimpleUpdater
bool RunUpdater()
{
	const int MAX_CMD = 32767;
	wchar_t pPath[MAX_PATH];
	wchar_t commandLine[MAX_CMD] = L"\"";

	GetModuleFileName(0, pPath, MAX_PATH);
	PathRemoveFileSpec(pPath);
	PathAppend(pPath, L"\\StupidSimpleUpdater.exe");
	wcscat_s(commandLine, pPath);
	wcscat_s(commandLine, L"\" -appinv");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (CreateProcess(pPath, commandLine, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi))
	{
		return true;
	}
	else 
	{
		return false;
	}
}


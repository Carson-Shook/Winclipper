// Entry point for the application.
#include "stdafx.h"
#include "Winclipper.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	const wchar_t uniqueApplicationMutex[] = L"com_carsonshook_winclipper";
	HANDLE hHandle = CreateMutexW(nullptr, TRUE, uniqueApplicationMutex);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 1;
	}

	RunUpdater();

	const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	MSG msg;

	if (SUCCEEDED(hr))
	{
		try
		{
			if (RegistryUtilities::ClipboardHistoryEnabled() && !RegistryUtilities::ClipboardStartupWarningDisabled())
			{
				int choice = MessageBoxW(nullptr,
					L"It looks like you have Windows Clipboard History enabled. It can cause issues with Winclipper. Would you like to turn it off now? It can always be re-enabled in the Settings app.\r\n\r\nIf you want to leave it on, choose \"No\" to ignore this warning.", L"Winclipper",
					MB_YESNOCANCEL | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND);

				if (choice == IDYES)
				{
					RegistryUtilities::DisableClipboardHistory();
				}
				else if (choice == IDNO)
				{
					RegistryUtilities::DisableClipboardStartupWarning();
				}
				else
				{
					CoUninitialize();
					return 1;
				}
			}

			UserSettings::InitializeSettings();

			SettingsWindow settingsWindow = SettingsWindow(hInstance);
			MainWindow mainWindow = MainWindow(hInstance, settingsWindow);

			hInst = hInstance; // Store instance handle in our global variable

			HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINCSETTINGS));

			// Main message loop:
			while (GetMessage(&msg, nullptr, 0, 0))
			{
				if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				{
					if (!IsDialogMessage(GetAncestor(msg.hwnd, GA_ROOTOWNER), &msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}

			CoUninitialize();
		}
		catch (const std::exception& e)
		{
			CoUninitialize();
			MessageBoxA(nullptr, (std::string("A fatal error occured:\r\n\"") + e.what() + "\"\r\n\r\nWinclipper will now close.").c_str(), "Winclipper", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
		}
	}

	if (hHandle != nullptr)
	{
		ReleaseMutex(hHandle);
		CloseHandle(hHandle);
	}
    return 0;
}

// Execute StupidSimpleUpdater
bool RunUpdater()
{
	const size_t MAX_CMD = 32767;
	wchar_t path[MAX_PATH];
	wchar_t * commandLine = new wchar_t[MAX_CMD];

	if (!GetModuleFileNameW(nullptr, path, MAX_PATH))
	{
		// Since we're targeting Win7, if MAX_PATH is
		// exceeded then there isn't much else we can do.
		return false;
	}

	PathRemoveFileSpecW(path);
	
	if (!PathAppendW(path, L"\\StupidSimpleUpdater.exe"))
	{
		// Same as above
		return false;
	}

	wcscpy_s(commandLine, MAX_CMD, (L"\"" + std::wstring(path) + L"\" -appinv").c_str());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	SecureZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	SecureZeroMemory(&pi, sizeof(pi));

	if (!CreateProcessW(path, commandLine, nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi))
	{
		return false;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	delete[] commandLine;
	return true;
}


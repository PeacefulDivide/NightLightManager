// main.cpp

#pragma comment(linker, "/SUBSYSTEM:windows")
#include "NightLightManager.h"
#include <shellapi.h>
#include "resource.h"
#include <Windows.h>
#include <tchar.h>
#include <CommCtrl.h>


#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 2001
#define ID_TRAY_SETTINGS 2002


std::vector<std::wstring> excludedGames = { L"EldenRing.exe" };
bool isGamingMode = false;

// Default saturation 50%
float g_RedIntensity = 0.5f;

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Basic save function
void SaveConfig() {
	std::wofstream file("config.txt");
	if (file.is_open()) {
		file << g_RedIntensity << std::endl; // Save slider value
		for (const auto& g : excludedGames) file << g << std::endl;
	}
}

// Basic load function
void LoadConfig() {
	std::wifstream file("config.txt");
	if (file.is_open()) {
		excludedGames.clear();
		std::wstring line;

		if (std::getline(file, line)) {
			try {
				g_RedIntensity = std::stof(line);
			}
			catch (...) {
				g_RedIntensity = 0.5f; // Default if file corruption occurss
			}
		}
		while (std::getline(file, line)) {
			if (!line.empty()) {
				excludedGames.push_back(line);
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_POWERBROADCAST:
		if (wParam == PBT_APMRESUMESUSPEND) {
			// When windows wakes up
			// Re apply last known gamma settings
			NightLightManager::SetState(true, g_RedIntensity);
			OutputDebugStringW(L"System resumed from sleep: Applying Tint.\n");
		}
		break;

	case WM_DISPLAYCHANGE:
		// Re apply tint if resolution changes
		NightLightManager::SetState(true, g_RedIntensity);
		break;

	case WM_TRAYICON:
		if (lParam == WM_RBUTTONUP) {
			POINT curPoint;
			GetCursorPos(&curPoint);
			HMENU hMenu = CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, ID_TRAY_SETTINGS, L"Settings (Edit List)");
			AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL);
			DestroyMenu(hMenu);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_TRAY_EXIT) {
			PostQuitMessage(0);
		}
		if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN_DIALOG), hWnd, SettingsDialogProc);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Working Directory
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	// Remove the filename to get just the directory
	for (int i = wcslen(path) - 1; i >= 0; i--) {
		if (path[i] == L'\\') {
			path[i] = L'\0';
			break;
		}
	}
	SetCurrentDirectoryW(path);
	
	SetProcessDPIAware();

	// Load data first
	LoadConfig();
	
	// Hidden window for messaging
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"NightLightWatcher";
	wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	RegisterClass(&wc);
	HWND hWnd = CreateWindow(L"NightLightWatcher", NULL, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

	// System tray icon
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	lstrcpy(nid.szTip, L"Night Light Game Watcher");
	Shell_NotifyIcon(NIM_ADD, &nid);

	// Launch settings UI on startup
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), hWnd, SettingsDialogProc);

	// Background loop
	MSG msg;
	ULONGLONG lastCheck = 0;

	NightLightManager::SetState(true, g_RedIntensity);

	while (true) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return 0;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Run every 2 seconds
		if (GetTickCount64() - lastCheck > 2000) {
			bool currentlyGaming = NightLightManager::IsGameRunning(excludedGames);

			// DEBUG
			if (currentlyGaming && !isGamingMode) {
				OutputDebugStringW(L"Night Light Manager: Game Detected! Tint Removed\n");
				NightLightManager::SetState(false); // turns off for game
				isGamingMode = true;
			}
				
			else if (!currentlyGaming && isGamingMode) {
				NightLightManager::SetState(true, g_RedIntensity); // Turns back on when not gaming
				isGamingMode = false;
			}
			lastCheck = GetTickCount64();
		}
		Sleep(10); // Lower CPU usage
	}

	NightLightManager::SetState(true, g_RedIntensity);
	Shell_NotifyIcon(NIM_DELETE, &nid);
	return 0;
}
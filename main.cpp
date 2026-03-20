#pragma comment(linker, "/SUBSYSTEM:windows")
#include "NightLightManager.h"
#include <shellapi.h>
#include "resource.h"
#include <Windows.h>
#include <tchar.h>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 2001
#define ID_TRAY_SETTINGS 2002

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

std::vector<std::wstring> excludedGames = { L"EldenRing.exe" };
bool isGamingMode = false;

// Basic save function
void SaveConfig() {
	std::wofstream file("config.txt");
	for (const auto& g : excludedGames) file << g << std::endl;

}

// Basic load function
void LoadConfig() {
	std::wifstream file("config.txt");
	if (file.is_open()) {
		excludedGames.clear();
		std::wstring line;
		while (std::getline(file, line)) {
			if (!line.empty()) {
				excludedGames.push_back(line);
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
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
	// Load data first
	LoadConfig();
	
	// Hidden window for messaging
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"NightLightWatcher";
	RegisterClass(&wc);
	HWND hWnd = CreateWindow(L"NightLightWatcher", NULL, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

	// System tray icon
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	lstrcpy(nid.szTip, L"Night Light Game Watcher");
	Shell_NotifyIcon(NIM_ADD, &nid);

	// Launch settings UI on startup
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), hWnd, SettingsDialogProc);

	// Background loop
	MSG msg;
	DWORD64 lastCheck = 0;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Run every 2 seconds
		if (GetTickCount64() - lastCheck > 2000) {
			bool currentlyGaming = NightLightManager::IsGameRunning(excludedGames);

			if (currentlyGaming && !isGamingMode) {
				NightLightManager::SetState(false); // turns off for game
				isGamingMode = true;
			}
			else if (!currentlyGaming && isGamingMode) {
				NightLightManager::SetState(true); // Turns back on when not gaming
				isGamingMode = false;
			}
			lastCheck = (DWORD)GetTickCount64();
		}
		Sleep(10); // Lower CPU usage
	}

	Shell_NotifyIcon(NIM_DELETE, &nid);
	return 0;
}
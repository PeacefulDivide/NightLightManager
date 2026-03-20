#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <Psapi.h>
#include <fstream>

class NightLightManager {
public:
	// Toggles windows 11 night light registry key
	static void SetState(bool active) {
		HKEY hKey;
		const wchar_t* subkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Store\\DefaultNetworkConfiguration\\Cloud\\default$windows.data.bluelightreduction.settings\\Current";

		if (RegOpenKeyExW(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
			DWORD size;
			if (RegQueryValueExW(hKey, L"Data", NULL, NULL, NULL, &size) == ERROR_SUCCESS) {
				std::vector<BYTE> data(size);
				if (RegQueryValueExW(hKey, L"Data", NULL, NULL, data.data(), &size) == ERROR_SUCCESS) {
					// Byte 18: 0x15 on, 0x10 off.
					data[18] = active ? 0x15 : 0x10;
					RegSetValueExW(hKey, L"Data", 0, REG_BINARY, data.data(), size);

					// Force windows to update night light
					SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Policy", SMTO_ABORTIFHUNG, 5000, NULL);

				}
			}
			RegCloseKey(hKey);
		}
	}

	// Check if current window is in exclusion list
	static bool IsGameRunning(const std::vector<std::wstring>& list) {
		HWND hwnd = GetForegroundWindow();
		if (!hwnd) return false;

		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);
		HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);

		if (hProc) {
			wchar_t buffer[MAX_PATH];
			if (GetModuleBaseNameW(hProc, NULL, buffer, MAX_PATH)) {
				std::wstring currentProc = buffer;
				for (const auto& game : list) {
					if (_wcsicmp(currentProc.c_str(), game.c_str()) == 0) {
						CloseHandle(hProc);
						return true;
					}
				}
			}
			CloseHandle(hProc);
		}
		return false;
	}
};
void SaveConfig();
void LoadConfig();
void SetAutoStart(bool enable);
bool IsAutoStartEnabled();

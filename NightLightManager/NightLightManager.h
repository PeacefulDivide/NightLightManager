// NightLightManager.h

#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <Psapi.h>
#include <fstream>
#include <tlhelp32.h>

#pragma comment(lib, "Psapi.lib")

// Class for changing Gamma
class NightLightManager {
public:
	// Calculations
	static void CalculateRamp(bool active, float redIntensity, WORD ramp[3][256]) {
		float intensity = (redIntensity < 0.0f) ? 0.0f : (redIntensity > 1.0f) ? 1.0f : redIntensity;

		float bMult = 1.0f - intensity * 0.9f;
		float gMult = 1.0f - intensity * 0.5f;

		for (int i = 0; i < 256; i++) {
			// Baseline 0-65535
			double baseValue = (double)(i * 257);

			if (!active) {
				ramp[0][i] = ramp[1][i] = ramp[2][i] = (WORD)baseValue;
			}
			else {

				ramp[0][i] = (WORD)baseValue;

				// Force green and blue to stay within range
				ramp[1][i] = (WORD)(baseValue * (double)gMult);
				ramp[2][i] = (WORD)(baseValue * (double)bMult);

				if (i > 0) {

					if (ramp[1][i] < ramp[1][i - 1]) ramp[1][i] = ramp[1][i - 1];
					if (ramp[2][i] < ramp[2][i - 1]) ramp[2][i] = ramp[2][i - 1];

				}
			}
		}

		// Apply the new color profile to the GPU
		ramp[0][0] = ramp[1][0] = ramp[2][0] = 0;

		if (!active) {
			ramp[0][255] = ramp[1][255] = ramp[2][255] = 65535;
		}
	}
	// Apply filter to all monitors
	static BOOL CALLBACK ApplyToAllMonitors(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
		// dwData passes ramp table
		WORD(*ramp)[256] = (WORD(*)[256])dwData;

		MONITORINFOEX mi;
		mi.cbSize = sizeof(mi);
		if (GetMonitorInfo(hMonitor, &mi)) {
			// Create DC for each monitor
			HDC hdc = CreateDC(mi.szDevice, NULL, NULL, NULL);
			if (hdc) {
				if (!SetDeviceGammaRamp(hdc, ramp)) {
					DWORD error = GetLastError();
					wchar_t errBuf[256];
					swprintf_s(errBuf, L"NLM: Gamma failed on %Ts. Error code: %lu\n", mi.szDevice, error);
					OutputDebugStringW(errBuf);
				}
				DeleteDC(hdc);

			}
			else {
				OutputDebugStringW(L"NLM: Failed to create DC for monitor.\n");
			}
		}
		// Continue to next monitor
		return TRUE; 
	}
	static void SetState(bool active, float redIntensity = 0.5f) {
		// Set gamma state
		WORD ramp[3][256];
		CalculateRamp(active, redIntensity, ramp);

        if (!EnumDisplayMonitors(NULL, NULL, ApplyToAllMonitors, (LPARAM)ramp)) {
			OutputDebugStringW(L"NLM: Monitor Enumeration Failed.\n");
     
        }
    }

	// Check if current window is in exclusion list
	static bool IsGameRunning(const std::vector<std::wstring>& list) {
		// take snap shot of all processes
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hSnap == INVALID_HANDLE_VALUE) return false;


		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(hSnap, &pe32)) {
			do {
				// Check every running process against list
				for (const auto& game : list) {
					if (_wcsicmp(pe32.szExeFile, game.c_str()) == 0) {
						CloseHandle(hSnap);
						return true; // Found
					}
				}
			} while (Process32NextW(hSnap, &pe32));
		}
		CloseHandle(hSnap);
		return false; // Game isnt running
	}
};
void SaveConfig(const std::wstring& filename = L"config.txt");
void LoadConfig(const std::wstring& filename = L"config.txt");
void SetAutoStart(bool enable);
bool IsAutoStartEnabled();
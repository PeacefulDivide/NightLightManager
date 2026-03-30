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

		float aggressiveIntensity = redIntensity;
		float bMult = 1.0f - aggressiveIntensity;
		float gMult = 1.0f - (aggressiveIntensity * 0.95f);

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
	
	static void SetState(bool active, float redIntensity = 0.5f) {
		// Get the Device Context for the primary monitor
		HDC hdc = GetDC(NULL);
		if (!hdc) return;

		WORD ramp[3][256];

		CalculateRamp(active, redIntensity, ramp);

        if (!SetDeviceGammaRamp(hdc, ramp)) {
            DWORD err = GetLastError();
            wchar_t buf[256];
            swprintf_s(buf, L"NLM: Gamma Failed. Error Code: %lu\n", err);
            OutputDebugStringW(buf);
        }
        else {
            OutputDebugStringW(active ? L"NLM: Gamma Warmth Applied!\n" : L"NLM: Gamma Reset.\n");
        }

        ReleaseDC(NULL, hdc);
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
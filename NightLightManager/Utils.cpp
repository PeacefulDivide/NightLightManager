// Utils.cpp

#include <iostream>
#include "NightLightManager.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>

extern std::vector<std::wstring> excludedGames;

void SetAutoStart(bool enable) {
    HKEY hKey;
    const wchar_t* runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const wchar_t* appName = L"NightLightManager";

    if (RegOpenKeyExW(HKEY_CURRENT_USER, runKey, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (enable) {
            wchar_t szPath[MAX_PATH];
            GetModuleFileNameW(NULL, szPath, MAX_PATH);
            std::wstring cmd = L"\"" + std::wstring(szPath) + L"\" --silent";
            RegSetValueExW(hKey, L"NightLightManager", 0, REG_SZ, (BYTE*)szPath, (DWORD)((wcslen(szPath) + 1) * sizeof(wchar_t)));
        }
        else {
            RegDeleteValueW(hKey, appName);
        }
        RegCloseKey(hKey);
    }
}

bool IsAutoStartEnabled() {
    HKEY hKey;
    const wchar_t* runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    bool enabled = false;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, runKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"NightLightManager", NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            enabled = true;
        }
        RegCloseKey(hKey);
    }
    return enabled;
}
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
    if (RegOpenKeyExW(HKEY_CURRENT_USER, runKey, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (enable) {
            wchar_t path[MAX_PATH];
            GetModuleFileNameW(NULL, path, MAX_PATH);
            RegSetValueExW(hKey, L"NightLightManager", 0, REG_SZ, (BYTE*)path, (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
        }
        else {
            RegDeleteValueW(hKey, L"NightLightManager");
        }
        RegCloseKey(hKey);
    }
}

bool IsAutoStartEnabled() {
    HKEY hKey;
    const wchar_t* runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegOpenKeyExW(HKEY_CURRENT_USER, runKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        LSTATUS status = RegQueryValueExW(hKey, L"NightLightManager", NULL, NULL, NULL, NULL);
        RegCloseKey(hKey);
        return (status == ERROR_SUCCESS);
    }
    return false;
}
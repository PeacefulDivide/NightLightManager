// SettingsUI.cpp

#include "NightLightManager.h"
#include "resource.h"
#include <commdlg.h> 
#include <vector>
#include <string>
#include <CommCtrl.h>

#pragma comment(lib, "Comdlg32.lib")

// Global list for UI to interact with
extern std::vector<std::wstring> excludedGames;
extern bool isGamingMode;
extern float g_RedIntensity;

// Declarations for helper functions
//void SaveConfig();
void SetAutoStart(bool enable);
bool IsAutoStartEnabled();

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_INITDIALOG: {

		// Load Icon
		HICON hMainIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));

		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hMainIcon);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hMainIcon);

		// Set checkbox state in registry
		CheckDlgButton(hDlg, IDC_CHK_STARTUP, IsAutoStartEnabled() ? BST_CHECKED : BST_UNCHECKED);

		// Populate list with existing games on startup
		SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_RESETCONTENT, 0, 0);
		for (const auto& game : excludedGames) {
			SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_ADDSTRING, 0, (LPARAM)game.c_str());
		}

		// Initialize Slider
		SendDlgItemMessage(hDlg, IDC_RED_SLIDER, TBM_SETRANGE, TRUE, MAKELONG(0, 1000));
		SendDlgItemMessage(hDlg, IDC_RED_SLIDER, TBM_SETPOS, TRUE, (LPARAM)(g_RedIntensity * 1000));

		return (INT_PTR)TRUE;
	}

	case WM_HSCROLL: {
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_RED_SLIDER)) {
			int pos = (int)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			g_RedIntensity = (float)pos / 2000.0f;

			// Live preview
			if (!isGamingMode) {
				NightLightManager::SetState(true, g_RedIntensity);
			}
		}
		break;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDC_BTN_BROWSE: {
			OPENFILENAME ofn;
			wchar_t szFile[MAX_PATH] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn)) {
				std::wstring fullPath(szFile);
				size_t lastSlash = fullPath.find_last_of(L"\\");
				std::wstring fileName = (lastSlash == std::wstring::npos) ? fullPath : fullPath.substr(lastSlash + 1);

				// Put filename in text box and trigger add logic
				SetDlgItemText(hDlg, IDC_GAME_INPUT, fileName.c_str());
				SendMessage(hDlg, WM_COMMAND, IDC_BTN_ADD, 0);
			}
			break;
		}

		case IDC_BTN_ADD: {
			wchar_t buffer[256];
			GetDlgItemText(hDlg, IDC_GAME_INPUT, buffer, 256);

			if (wcslen(buffer) > 0) {
				bool exists = false;
				for (const auto& game : excludedGames) {
					if (_wcsicmp(game.c_str(), buffer) == 0) {
						exists = true;
						break;
					}
				}
				if (!exists) {
					excludedGames.push_back(buffer);
					SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_ADDSTRING, 0, (LPARAM)buffer);
					SetDlgItemText(hDlg, IDC_GAME_INPUT, L"");
					SaveConfig();
				}
				else {
					// Alert game is already in list
					SetDlgItemText(hDlg, IDC_GAME_INPUT, L"");
					MessageBox(hDlg, L"This game is already in exlusion list.", L"Duplicate Entry", MB_OK | MB_ICONINFORMATION);

				}


			}
			break;
		}
		
		case IDC_BTN_REMOVE: {// close button
			int sel = (int)SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_GETCURSEL, 0, 0);
			if (sel != LB_ERR) {
				// Remove from list
				SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_DELETESTRING, sel, 0);
				// Remove global 
				excludedGames.erase(excludedGames.begin() + sel);
				// Update config files
				SaveConfig();
			}
			break;
		}

		case IDC_CHK_STARTUP: {
			// Checkbox for startup
			bool isChecked = (IsDlgButtonChecked(hDlg, IDC_CHK_STARTUP) == BST_CHECKED);
			SetAutoStart(isChecked);
			break;
		}
		
		// Okay button
		case IDOK:
			SaveConfig();
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		
			// Cancel button
		case IDCANCEL:
			LoadConfig();
			NightLightManager::SetState(true, g_RedIntensity);
			EndDialog(hDlg, IDCANCEL);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
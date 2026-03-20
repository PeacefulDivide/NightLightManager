#include "NightLightManager.h"
#include "resource.h"
#include <commdlg.h> 
#include <vector>
#include <string>

#pragma comment(lib, "Comdlg32.lib")

// Global list for UI to interact with
extern std::vector<std::wstring> excludedGames;

// Declarations for helper functions
void SaveConfig();
void SetAutoStart(bool enable);
bool IsAutoStartEnabled();

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		// Set checkbox state in registry
		CheckDlgButton(hDlg, IDC_CHK_STARTUP, IsAutoStartEnabled() ? BST_CHECKED : BST_UNCHECKED);
		// Populate list with existing games on startup
		SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_RESETCONTENT, 0, 0);
		for (const auto& game : excludedGames) {
			SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_ADDSTRING, 0, (LPARAM)game.c_str());

		}
		return (INT_PTR)TRUE;

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
			ofn.lpstrFilter = L"Executable Files (*.exe\0*.exe\0All Files (*.*)\0*.*\0";
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
				excludedGames.push_back(buffer);
				SendDlgItemMessage(hDlg, IDC_GAME_LIST, LB_ADDSTRING, 0, (LPARAM)buffer);
				SetDlgItemText(hDlg, IDC_GAME_INPUT, L"");
				SaveConfig();

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

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
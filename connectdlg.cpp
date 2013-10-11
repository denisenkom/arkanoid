#include "StdAfx.h"

CHAR ConnectDlg::address[256] = "localhost";

BOOL CALLBACK AddressDialogProc(HWND hwndDlg,	UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WORD wNotifyCode;
	WORD wID;
	HWND hwndCtl;

	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hwndDlg, IDC_ADDRESSEDIT), ConnectDlg::address);
		return 0;

	case WM_COMMAND:
		wNotifyCode = HIWORD(wParam);
		wID = LOWORD(wParam);
		hwndCtl = (HWND)lParam;
		if (wNotifyCode == 0 || wNotifyCode == 1) {
			switch (wID) {
			case IDOK:
				EndDialog(hwndDlg, TRUE);
				return IDOK;

			case IDCANCEL:
				EndDialog(hwndDlg, FALSE);
				return IDCANCEL;

			default:
				return 0;
			}
		}
		else {
			switch (wID) {
			case IDC_ADDRESSEDIT:
				switch (wNotifyCode) {
				case EN_CHANGE:
					GetWindowText(hwndCtl, ConnectDlg::address, sizeof(ConnectDlg::address));
					return 1;

				default:
					return 0;
				}

			default:
				return 0;
			}
		}

	default:
		return 0;
	}
}

int ConnectDlg::Run() {
	return DialogBox(Arkanoid::hInstance, MAKEINTRESOURCE(IDD_ADDRESSDIALOG),
		Screen::hWnd, &AddressDialogProc);
}
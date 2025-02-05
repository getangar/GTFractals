#include "dialog.h"

// Callback for the dialog
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		WCHAR buffer[100];

		// Set xmin
		swprintf(buffer, 100, L"%lf", xmin);
		SetDlgItemText(hDlg, IDC_XMIN, buffer);

		// Set ymin
		swprintf(buffer, 100, L"%lf", ymin);
		SetDlgItemText(hDlg, IDC_YMIN, buffer);

		// Set xmax
		swprintf(buffer, 100, L"%lf", xmax);
		SetDlgItemText(hDlg, IDC_XMAX, buffer);

		// Set ymax
		swprintf(buffer, 100, L"%lf", ymax);
		SetDlgItemText(hDlg, IDC_YMAX, buffer);

		// Set max_iter
		swprintf(buffer, 100, L"%d", max_iter);
		SetDlgItemText(hDlg, IDC_ITER, buffer);

		// Set px
		swprintf(buffer, 100, L"%lf", px);
		SetDlgItemText(hDlg, IDC_PX, buffer);

		// Set py
		swprintf(buffer, 100, L"%lf", py);
		SetDlgItemText(hDlg, IDC_PY, buffer);

		// Set Julia checkbox
		CheckDlgButton(hDlg, IDC_JULIA, isJulia ? BST_CHECKED : BST_UNCHECKED);

		// Get the handle to the dropdown list
		HWND hCombo = GetDlgItem(hDlg, IDC_COLOR_PATTERN);

		// Add items to the dropdown
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Modern");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Dark Gradient");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Vintage");

		// Set default selection
		SendMessage(hCombo, CB_SETCURSEL, 2, 0); // Selects "Vintage"

		return TRUE;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			WCHAR buffer[100];

			// Read xmin
			GetDlgItemText(hDlg, IDC_XMIN, buffer, 100);
			xmin = wcstod(buffer, NULL);

			// Read ymin
			GetDlgItemText(hDlg, IDC_YMIN, buffer, 100);
			ymin = wcstod(buffer, NULL);

			// Read xmax
			GetDlgItemText(hDlg, IDC_XMAX, buffer, 100);
			xmax = wcstod(buffer, NULL);

			// Read ymax
			GetDlgItemText(hDlg, IDC_YMAX, buffer, 100);
			ymax = wcstod(buffer, NULL);

			// Read max_iter
			GetDlgItemText(hDlg, IDC_ITER, buffer, 100);
			max_iter = _wtoi(buffer);

			// Read px
			GetDlgItemText(hDlg, IDC_PX, buffer, 100);
			px = wcstod(buffer, NULL);

			// Read py
			GetDlgItemText(hDlg, IDC_PY, buffer, 100);

			// Read Julia checkbox
			isJulia = IsDlgButtonChecked(hDlg, IDC_JULIA) == BST_CHECKED;

			// Get selected color pattern
			int selectedIndex = (int)SendDlgItemMessage(hDlg, IDC_COLOR_PATTERN, CB_GETCURSEL, 0, 0);

			// Save selected index in a global or static variable
			selectedColorPattern = selectedIndex;

			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

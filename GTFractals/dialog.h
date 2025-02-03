#ifndef DIALOG_H
#define DIALOG_H

#include <windows.h>

// Callback for the dialog
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Callback for the About dialog
BOOL CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif
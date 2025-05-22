#ifndef DIALOG_H
#define DIALOG_H

#include <windows.h>
#include <stdio.h>
#include "global.h"
#include "resource.h"


// Callback for the dialog
LRESULT CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Callback for the About dialog
LRESULT CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif
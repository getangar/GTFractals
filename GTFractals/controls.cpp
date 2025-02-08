#include "controls.h"

HWND hStatusBar = NULL;   // Definition of the variables
HWND hProgressBar = NULL; // Definition of the variables

void CreateStatusBar(HWND hwnd) {
	// Create the status bar
	hStatusBar = CreateWindowEx(
		0, STATUSCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0, 0, 0, 0,
		hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

	if (!hStatusBar) {
		MessageBox(hwnd, L"Failed to create Status Bar!", L"Error", MB_ICONERROR);
		return;
	}

	// Set the parts of the status bar
	int statusWidths[] = { 200, 400, 600, 800, -1 };
	SendMessage(hStatusBar, SB_SETPARTS, 5, (LPARAM)statusWidths);

	RECT rcStatus;
	GetClientRect(hStatusBar, &rcStatus);
	
	hProgressBar = CreateWindowEx(
		0, PROGRESS_CLASS, NULL,
		WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
		rcStatus.right - 150, 2, 140, rcStatus.bottom - 4,
		hStatusBar, NULL, GetModuleHandle(NULL), NULL);

	if (!hProgressBar) {
		MessageBox(hwnd, L"Failed to create Progress Bar!", L"Error", MB_ICONERROR);
		return;
	}

	SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessage(hProgressBar, PBM_SETPOS, 0, 0);

	SendMessage(hwnd, WM_SIZE, 0, 0);
}

void UpdateStatusBar() {
	wchar_t buffer[256];

	// Display the current values of xmin and ymin
	swprintf(buffer, 256, L"xmin: %.5f, ymin: %.5f", xmin, ymin);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	// Display the current values of xmax and ymax
	swprintf(buffer, 256, L"xmax: %.5f, ymax: %.5f", xmax, ymax);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	swprintf(buffer, 256, L"Iterations: %d", max_iter);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	const wchar_t * colorScheme = (selectedColorPattern == 0) ? L"Modern" :
								  (selectedColorPattern == 1) ? L"Dark Gradient" :
								  (selectedColorPattern == 2) ? L"Vintage" : L"Unknown";

	swprintf(buffer, 256, L"Color Scheme: %s", colorScheme);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);
}
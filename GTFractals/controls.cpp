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

	// Set the status bar parts
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	int width = rcClient.right;

	int statusWidths[] = {
		width * 1 / 6,   // xmin
		width * 2 / 6,   // ymin
		width * 3 / 6,   // xmax
		width * 4 / 6,   // ymax
		width * 5 / 6,   // Iterations
		-1               // Progress Bar (last column)
	};

	SendMessage(hStatusBar, SB_SETPARTS, 6, (LPARAM)statusWidths);

	// Create the progress bar
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
}

void UpdateStatusBarText() {
	wchar_t buffer[128];

	// Update xmin
	swprintf(buffer, 128, L"X-Min: %.5f", xmin);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	// Update ymin
	swprintf(buffer, 128, L"Y-Min: %.5f", ymin);
	SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)buffer);

	// Update xmax
	swprintf(buffer, 128, L"X-Max: %.5f", xmax);
	SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)buffer);

	// Update ymax
	swprintf(buffer, 128, L"Y-Maz: %.5f", ymax);
	SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)buffer);

	// Update Iterations
	swprintf(buffer, 128, L"Iterations: %d", max_iter);
	SendMessage(hStatusBar, SB_SETTEXT, 4, (LPARAM)buffer);
}
#include "controls.h"

HWND hStatusBar = NULL;		// Definition of the variables
HWND hProgressBar = NULL;	// Definition of the variables
HWND hToolBar = NULL;		// Definition of the global variable
HWND hRebar = NULL;			// Definition of the rebar


// Function to create the status bar
void CreateStatusBar(HWND hwnd) {
	// Create the status bar
	hStatusBar = CreateWindowEx(
		0, STATUSCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0, 0, 0, 0,
		hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

	if (!hStatusBar) {
		MessageBox(hwnd, "Failed to create Status Bar!", "Error", MB_ICONERROR);
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
		rcStatus.right - 100, 2, 100, rcStatus.bottom - 4,
		hStatusBar, NULL, GetModuleHandle(NULL), NULL);

	if (!hProgressBar) {
		MessageBox(hwnd, "Failed to create Progress Bar!", "Error", MB_ICONERROR);
		return;
	}

	// Set the range of the progress bar
	SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessage(hProgressBar, PBM_SETPOS, 0, 0);

	SendMessage(hwnd, WM_SIZE, 0, 0);
}

// Function to update the status bar
void UpdateStatusBar() {
	char buffer[128];

	// Display the current values of xmin and ymin
	sprintf(buffer, "xmin: %.5f, ymin: %.5f", xmin, ymin);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	// Display the current values of xmax and ymax
	sprintf(buffer, "xmax: %.5f, ymax: %.5f", xmax, ymax);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	// Display the current value of max_iter
	sprintf(buffer, "Iterations: %d", max_iter);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);	
}

void UpdateStatusBarText() {
	char buffer[128];

	sprintf(buffer, "X-Min: %.5f", xmin);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)buffer);

	// Update ymin
	sprintf(buffer, "Y-Min: %.5f", ymin);
	SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)buffer);

	// Update xmax
	sprintf(buffer, "X-Max: %.5f", xmax);
	SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)buffer);

	// Update ymax
	sprintf(buffer, "Y-Max: %.5f", ymax);
	SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)buffer);

	// Update Iterations
	sprintf(buffer, "Iterations: %d", max_iter);
	SendMessage(hStatusBar, SB_SETTEXT, 4, (LPARAM)buffer);
}

// Function to create the toolbar
void CreateToolBar(HWND hwnd) {
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	TBBUTTON tbb[] = {
		{ MAKELONG(STD_FILENEW,  0), ID_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (int)"Reset" },
		{ MAKELONG(STD_FILEOPEN, 0), ID_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (int)"Open" },
		{ MAKELONG(STD_FILESAVE, 0), ID_FILE_SAVE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (int)"Save" },
		{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0 }, // Separator
		{ MAKELONG(STD_PRINT, 0),    ID_FILE_PRINT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (int)"Print" },
		{ MAKELONG(STD_HELP,  0),    ID_HELP_ABOUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (int)"About" }
	};


	hToolBar = CreateToolbarEx(hwnd,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
		1, 0,
		HINST_COMMCTRL,
		IDB_STD_SMALL_COLOR,
		tbb,
		5, 0, 0 ,0, 0,
		sizeof(TBBUTTON));

	SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);	
}

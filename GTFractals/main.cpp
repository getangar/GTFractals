#include <windows.h>
#include <stdio.h>

#include "dialog.h"
#include "resource.h"
#include "global.h"

#include "Mandelbrot.h"
#include "Julia.h"
#include "utils.h"
#include "export.h"
#include "controls.h"


// Global variables
double xmin = -2.25, ymin = -1.5, xmax = 0.75, ymax = 1.5;	// Initial coordinates
double px = 0.0, py = -1.0;									// Julia set parameters
int max_iter = 250;											// Maximum number of iterations
bool isThreadRunning = false;								// Check if any thread is running
int selectedColorPattern = 2;								// Default color to "Vintage"

// Flags to control the application
bool isJulia = false;					// Indicates if the Julia set is being displayed
bool isFirstJulia = true;				// Indicates if it's the first time the Julia set is displayed
bool isSelecting = false;				// Indicates if a selection is in progress
bool isResizing = false;				// Indicates if the window is being resized
int prevWidth = 0, prevHeight = 0;		// Previous window dimensions
POINT startPoint, endPoint;				// Start and end coordinates of the selection
RECT selectionRect = { 0, 0, 0, 0 };	// Selection rectangle

// Structure to pass parameters to the thread function
typedef struct {
	HWND hwnd;
} FractalParams;

// Function prototypes for the main window
void AddMenus(HWND);			// Add menus to the window
void UpdateFractal(HWND);		// Update the fractal based on the selection rectangle
void DrawSelectionRect(HWND);	// Draw the selection rectangle

// Function prototypes for the thread functions
DWORD WINAPI MandelbrotThread(LPVOID);	// Thread function for the Mandelbrot set
DWORD WINAPI JuliaThread(LPVOID);		// Thread function for the Julia set

// Function prototypes for the thread management
void StartMandelbrotThread(HWND);
void StartJuliaThread(HWND);

// Function prototypes for the fractal drawing
void ResetMandelbrot(HWND);

// Function prototypes for the save file handling
bool ShowSaveFileDialog(HWND, WCHAR*, DWORD);

// Function prototypes for the open file handling
bool ShowOpenFileDialog(HWND, WCHAR*, DWORD);

// Function prototypes for resize the window
bool ResizeWindow(LPARAM, WPARAM);

// Function prototypes for messages handling
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	LPCWSTR CLASS_NAME = L"GTFractals";  // Window class name

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// Load the icon
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

	if (!RegisterClassEx(&wc)) {
		DWORD error = GetLastError();
		wchar_t errorMessage[256];
		wsprintf(errorMessage, L"Window Registration Failed! Error Code: %d", error);
		MessageBox(NULL, errorMessage, L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"GTFractals",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
		NULL, NULL, hInstance, NULL);

	if (!hwnd) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Add menus to the window
	AddMenus(hwnd);
	ShowWindow(hwnd, nCmdShow);

	// Save the initial window dimensions
	RECT rect;
	GetClientRect(hwnd, &rect);
	prevWidth = rect.right;
	prevHeight = rect.bottom;

	// Message loop
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


// Main Menus
void AddMenus(HWND hwnd) {
	HMENU hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hEditMenu = CreateMenu();
	HMENU hDrawMenu = CreateMenu();
	HMENU hHelpMenu = CreateMenu();

	// File Menu
	AppendMenu(hFileMenu, MF_STRING, 101, L"Open");
	AppendMenu(hFileMenu, MF_STRING, 102, L"Save");
	AppendMenu(hFileMenu, MF_STRING, 103, L"Save as...");
	AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFileMenu, MF_STRING, 104, L"Export image");
	AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFileMenu, MF_STRING, 105, L"Print");
	AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFileMenu, MF_STRING, 1, L"Exit");

	// Edit Menu
	AppendMenu(hEditMenu, MF_STRING, 106, L"Settings");

	// Draw Menu
	AppendMenu(hDrawMenu, MF_STRING, 2, L"Reset Image");

	// Help Menu
	AppendMenu(hHelpMenu, MF_STRING, 107, L"Help");
	AppendMenu(hHelpMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hHelpMenu, MF_STRING, 3, L"About");

	// Add submenus to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDrawMenu, L"Draw");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"?");

	SetMenu(hwnd, hMenu);
}

// Function to draw the Mandelbrot set in a separate thread
DWORD WINAPI MandelbrotThread(LPVOID lpParam) {
	DebugPrint("MandelbrotThread started\n");

	FractalParams* params = (FractalParams*)lpParam;
	HWND hwnd = params->hwnd;

	DrawMandelbrot(hwnd);

	DebugPrint("MandelbrotThread finished\n");

	isThreadRunning = false; // Reset flag when the thread finishes
	free(params);
	return 0;
}

// Function to draw the Julia set in a separate thread
DWORD WINAPI JuliaThread(LPVOID lpParam) {
	DebugPrint("JuliaThread started\n");

	FractalParams* params = (FractalParams*)lpParam;
	HWND hwnd = params->hwnd;

	DrawJulia(hwnd);

	DebugPrint("JuliaThread finished\n");

	isThreadRunning = false; // Reset flag when the thread finishes
	free(params);
	return 0;
}

// Function to start the Mandelbrot thread
void StartMandelbrotThread(HWND hwnd) {
	if (isThreadRunning) {
		//DebugPrint("Thread already running, skipping...\n");
		return;
	}

	isThreadRunning = true;
	DebugPrint("Starting Mandelbrot thread\n");

	FractalParams* params = (FractalParams*)malloc(sizeof(FractalParams));
	if (params == NULL) {
		MessageBox(hwnd, L"Memory allocation failed", L"Error", MB_ICONERROR);
		return;
	}
	params->hwnd = hwnd;

	HANDLE threadHandle = CreateThread(NULL, 0, MandelbrotThread, params, 0, NULL);
	if (threadHandle == NULL) {
		DebugPrint("Error creating Mandelbrot thread\n");
		MessageBox(hwnd, L"Errore nella creazione del thread", L"Errore", MB_ICONERROR);
		free(params);
		isThreadRunning = false;
	}
	else {
		CloseHandle(threadHandle);
	}

	SendMessage(hProgressBar, PBM_SETPOS, 0, 0); // Resetta la progress bar

}

// Function to start the Julia thread
void StartJuliaThread(HWND hwnd) {
	if (isThreadRunning) {		
		return;
	}

	isThreadRunning = true;
	DebugPrint("Starting Julia thread\n");

	FractalParams* params = (FractalParams*)malloc(sizeof(FractalParams));
	if (params == NULL) {
		MessageBox(hwnd, L"Memory allocation failed", L"Error", MB_ICONERROR);
		return;
	}
	params->hwnd = hwnd;

	HANDLE threadHandle = CreateThread(NULL, 0, JuliaThread, params, 0, NULL);
	if (threadHandle == NULL) {
		DebugPrint("Error creating Julia thread\n");
		MessageBox(hwnd, L"Errore nella creazione del thread", L"Errore", MB_ICONERROR);
		free(params);
		isThreadRunning = false;
	}
	else {
		CloseHandle(threadHandle);
	}
}

// Function to draw the selection rectangle
void DrawSelectionRect(HWND hwnd) {
	HDC hdc = GetDC(hwnd); // Get the graphics context

	int oldMode = SetROP2(hdc, R2_NOTXORPEN); // XOR mode
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // Red border pen
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	Rectangle(hdc, selectionRect.left, selectionRect.top, selectionRect.right, selectionRect.bottom);

	SelectObject(hdc, hOldPen);
	SetROP2(hdc, oldMode);
	DeleteObject(hPen);
	ReleaseDC(hwnd, hdc);
}

// Function to update the fractal based on the selection rectangle
void UpdateFractal(HWND hwnd) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right;

	// Get the height of the toolbar
	RECT rcToolbar;
	GetWindowRect(hToolBar, &rcToolbar);
	int toolbarHeight = rcToolbar.bottom - rcToolbar.top;

	// Get the height of the status bar
	RECT rcStatus;
	GetWindowRect(hStatusBar, &rcStatus);
	int statusBarHeight = rcStatus.bottom - rcStatus.top;

	int height = rect.bottom - toolbarHeight - statusBarHeight;

	// Convert screen coordinates to complex plane coordinates
	double newXmin = xmin + (xmax - xmin) * selectionRect.left / width;
	double newXmax = xmin + (xmax - xmin) * selectionRect.right / width;
	double newYmin = ymin + (ymax - ymin) * (selectionRect.top - toolbarHeight) / height;
	double newYmax = ymin + (ymax - ymin) * (selectionRect.bottom - toolbarHeight) / height;

	xmin = newXmin;
	xmax = newXmax;
	ymin = newYmin;
	ymax = newYmax;

	InvalidateRect(hwnd, NULL, TRUE);

	// Update status bar
	UpdateStatusBarText();
}

// Function to reset the Mandelbrot set
void ResetMandelbrot(HWND hwnd) {
	// Reset to initial coordinates
	xmin = -2.25;
	xmax = 0.75;
	ymin = -1.5;
	ymax = 1.5;
	max_iter = 250;
	isJulia = false;

	// Force redraw
	InvalidateRect(hwnd, NULL, TRUE);
}

// Function to show the Save File dialog
bool ShowSaveFileDialog(HWND hwnd, WCHAR* filePath, DWORD filePathSize) {
	OPENFILENAME ofn = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"BMP Files\0*.bmp\0All Files\0*.*\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = filePathSize;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"bmp";

	return GetSaveFileName(&ofn);
}

// Function to show the Open File dialog
bool ShowOpenFileDialog(HWND hwnd, WCHAR* filePath, DWORD filePathSize) {
	OPENFILENAME ofn = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"BMP Files\0*.bmp\0All Files\0*.*\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = filePathSize;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = L"bmp";
	return GetOpenFileName(&ofn);
}

// Function to resize the window
bool ResizeWindow(LPARAM lParam, WPARAM wParam) {
	RECT* rect = (RECT*)lParam;

	// Define the correct aspect ratio (width / height)
	double aspectRatio = (xmax - xmin) / (ymax - ymin);

	// Get the current width and height of the resizing window
	int width = rect->right - rect->left;
	int height = rect->bottom - rect->top;

	// Adjust the window size while maintaining the aspect ratio
	switch (wParam) {
	case WMSZ_LEFT:
	case WMSZ_RIGHT:
		// Adjust height to maintain aspect ratio based on width
		rect->bottom = rect->top + (LONG)(width / aspectRatio);
		break;
	case WMSZ_TOP:
	case WMSZ_BOTTOM:
		// Adjust width to maintain aspect ratio based on height
		rect->right = rect->left + (LONG)(height * aspectRatio);
		break;
	case WMSZ_TOPLEFT:
	case WMSZ_TOPRIGHT:
	case WMSZ_BOTTOMLEFT:
	case WMSZ_BOTTOMRIGHT:
		// Adjust both width and height while maintaining the aspect ratio
		if ((double)width / height > aspectRatio) {
			rect->right = rect->left + (LONG)(height * aspectRatio);
		}
		else {
			rect->bottom = rect->top + (LONG)(width / aspectRatio);
		}
		break;
	}

	return TRUE; // Notify that we modified the window size
}

// Message processing function
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND: // Handle menu commands
		if (LOWORD(wParam) == 1) { // Exit
			PostQuitMessage(0);

			break;
		}
		else if (LOWORD(wParam) == 2 || LOWORD(wParam) == 3001) { // Reset
			ResetMandelbrot(hwnd);

			break;
		}
		else if (LOWORD(wParam) == 101 || LOWORD(wParam) == 3002) {
			WCHAR filePath[MAX_PATH] = { 0 };

			if (ShowOpenFileDialog(hwnd, filePath, MAX_PATH)) {
				if (!SaveBitmap(hwnd, filePath)) {
					MessageBox(hwnd, L"Failed to open image.", L"Error", MB_OK | MB_ICONERROR);
				}				
			}

			break;
			break;
		}
		else if (LOWORD(wParam) == 102) {
			break;
		}
		else if (LOWORD(wParam) == 103 || LOWORD(wParam) == 3003) {
			WCHAR filePath[MAX_PATH] = { 0 };

			if (ShowSaveFileDialog(hwnd, filePath, MAX_PATH)) {
				if (!SaveBitmap(hwnd, filePath)) {
					MessageBox(hwnd, L"Failed to save image.", L"Error", MB_OK | MB_ICONERROR);
				}				
			}

			break;
		}
		else if (LOWORD(wParam) == 105 || LOWORD(wParam) == 3004) {
			if (!PrintFractal(hwnd)) {
				MessageBox(hwnd, L"Failed to print.", L"Error", MB_OK | MB_ICONERROR);
			}			
			break;
		}
		else if (LOWORD(wParam) == 106) {
			if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc, 0) == IDOK) {
				InvalidateRect(hwnd, NULL, TRUE); // Force redraw
			}

			break;
		}
		else {
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hwnd, (DLGPROC)AboutDialogProc);						
		}
		break;
	case WM_ENTERSIZEMOVE: // The user is resizing the window
		isResizing = true;
		break;
	case WM_EXITSIZEMOVE: // The user has finished resizing the window
		isResizing = false;

		// Check if dimensions have changed
		RECT rect;
		GetClientRect(hwnd, &rect);
		if (rect.right != prevWidth || rect.bottom != prevHeight) {
			prevWidth = rect.right;
			prevHeight = rect.bottom;
			InvalidateRect(hwnd, NULL, TRUE); // Redraw only if dimensions changed
		}
		break;
	case WM_RBUTTONUP: // Right mouse button clicked
	{
		// Get mouse coordinates
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		RECT rect;
		GetClientRect(hwnd, &rect);
		int width = rect.right;
		int height = rect.bottom;

		// Convert screen coordinates to complex plane coordinates
		px = xmin + (xmax - xmin) * mouseX / width;
		py = ymin + (ymax - ymin) * mouseY / height;

		if (isFirstJulia) {
			// Reset to initial coordinates
			xmin = -1.5;
			xmax = 1.5;
			ymin = -1.5;
			ymax = 1.5;
			isFirstJulia = false;
		}

		// Set flag to display Julia set
		isJulia = true;

		// Start Julia set thread
		StartJuliaThread(hwnd);

		// Force redraw
		InvalidateRect(hwnd, NULL, TRUE);
	}

		break;
	case WM_LBUTTONDOWN: // Left mouse button clicked
		isSelecting = true;
		startPoint.x = LOWORD(lParam);
		startPoint.y = HIWORD(lParam);
		selectionRect.left = startPoint.x;
		selectionRect.top = startPoint.y;
		selectionRect.right = startPoint.x;
		selectionRect.bottom = startPoint.y;
		break;
	case WM_MOUSEMOVE: // Mouse moved
		if (isSelecting) {
			// Erase previous selection
			DrawSelectionRect(hwnd);

			// Get new selection end point
			int mouseX = LOWORD(lParam);
			int mouseY = HIWORD(lParam);

			// Calculate aspect ratio
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			double aspectRatio = (double)(clientRect.right - clientRect.left) / (clientRect.bottom - clientRect.top);

			// Compute new selection keeping aspect ratio
			int width = abs(mouseX - startPoint.x);
			int height = abs(mouseY - startPoint.y);

			if (width / (double)height > aspectRatio) {
				// Adjust height based on width
				height = (int)(width / aspectRatio);
			}
			else {
				// Adjust width based on height
				width = (int)(height * aspectRatio);
			}

			// Set new selection rectangle
			selectionRect.right = (mouseX > startPoint.x) ? startPoint.x + width : startPoint.x - width;
			selectionRect.bottom = (mouseY > startPoint.y) ? startPoint.y + height : startPoint.y - height;

			// Draw new selection
			DrawSelectionRect(hwnd);
		}
		break;
	case WM_LBUTTONUP: // Left mouse button released
		if (isSelecting) {
			isSelecting = false;
			DrawSelectionRect(hwnd); // Erase the rectangle
			UpdateFractal(hwnd);
		}
		break;
	case WM_PAINT: // Window needs to be redrawn
		if (!isResizing && !isJulia) { // Avoid drawing during resizing or moving			
			StartMandelbrotThread(hwnd);
		}
		else if (!isResizing && isJulia) {
			StartJuliaThread(hwnd);			
		}

		UpdateStatusBarText();

		break;
	case WM_CREATE: // Window is being created
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_BAR_CLASSES; // Initialize common controls
		InitCommonControlsEx(&icex);

		CreateToolBar(hwnd);	// Create the toolbar
		CreateStatusBar(hwnd);	// Create the status bar
		break;
	case WM_SIZE:
		// Resize the status bar
		if (hStatusBar) {
			SendMessage(hStatusBar, WM_SIZE, 0, 0);
		}
		if (hToolBar) {
			SendMessage(hToolBar, WM_SIZE, 0, 0); // Resize the toolbar
		}
		

		break;
	case WM_DESTROY: // Window is being destroyed
		PostQuitMessage(0);
		break;
	case WM_SIZING:
		ResizeWindow(lParam, wParam);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

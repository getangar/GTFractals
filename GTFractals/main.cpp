#include <windows.h>
#include <stdio.h>

#include "dialog.h"
#include "resource.h"
#include "global.h"

#include "Mandelbrot.h"
#include "Julia.h"
#include "utils.h"


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

// Function prototypes for messages handling
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// Entry point
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	LPCWSTR CLASS_NAME = L"GTFractals";  // Definizione corretta

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);  // Imposta esplicitamente cbSize
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Assicurati che sia impostato

	// Carica l'icona
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

	// Aggiungi menu e visualizza la finestra principale
	AddMenus(hwnd);
	ShowWindow(hwnd, nCmdShow);

	// Imposta le dimensioni iniziali
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

	isThreadRunning = false; // Reset flag quando il thread termina
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

	isThreadRunning = false; // Reset flag quando il thread termina
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
	int height = rect.bottom;

	// Convert screen coordinates to complex plane coordinates
	double newXmin = xmin + (xmax - xmin) * selectionRect.left / width;
	double newXmax = xmin + (xmax - xmin) * selectionRect.right / width;
	double newYmin = ymin + (ymax - ymin) * selectionRect.top / height;
	double newYmax = ymin + (ymax - ymin) * selectionRect.bottom / height;

	xmin = newXmin;
	xmax = newXmax;
	ymin = newYmin;
	ymax = newYmax;

	InvalidateRect(hwnd, NULL, TRUE); // Force redraw
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
	ofn.lpstrFilter = L"JPEG Files\0*.jpg\0All Files\0*.*\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = filePathSize;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"jpg";

	return GetSaveFileName(&ofn);
}

// Message processing function
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND: // Handle menu commands
		if (LOWORD(wParam) == 1) {
			PostQuitMessage(0);
		}
		else if (LOWORD(wParam) == 2) { // Reset
			ResetMandelbrot(hwnd);
		}
		else if (LOWORD(wParam) == 101) {
		}
		else if (LOWORD(wParam) == 102) {
		}
		else if (LOWORD(wParam) == 103) {
			WCHAR filePath[MAX_PATH] = { 0 };

			if (ShowSaveFileDialog(hwnd, filePath, MAX_PATH)) {
				HDC hdc = GetDC(hwnd);

				ReleaseDC(hwnd, hdc);
			}
		}
		else if (LOWORD(wParam) == 106) {
			if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc, 0) == IDOK) {
				InvalidateRect(hwnd, NULL, TRUE); // Force redraw
			}
		}
		else {
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hwnd, (DLGPROC)AboutDialogProc);			
			//MessageBox(hwnd, L"GTFractals\n\nA simple Mandelbrot set viewer\n(c)Copyright 2025 by Gennaro E. Tangari", L"About", MB_OK);
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
			DrawSelectionRect(hwnd); // Erase the previous rectangle
			endPoint.x = LOWORD(lParam);
			endPoint.y = HIWORD(lParam);
			selectionRect.right = endPoint.x;
			selectionRect.bottom = endPoint.y;
			DrawSelectionRect(hwnd); // Draw the new rectangle
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
			//DrawMandelbrot(hwnd);
			StartMandelbrotThread(hwnd);
		}
		else if (!isResizing && isJulia) {
			DrawJulia(hwnd);
		}
		break;
	case WM_DESTROY: // Window is being destroyed
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

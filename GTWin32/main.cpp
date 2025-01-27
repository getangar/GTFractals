#include <windows.h>
#include <stdio.h>

// Global variables
bool isSelecting = false;  // Indicates if a selection is in progress
bool isResizing = false;   // Indicates if the window is being resized
int prevWidth = 0, prevHeight = 0; // Previous window dimensions
POINT startPoint, endPoint; // Start and end coordinates of the selection
RECT selectionRect = { 0, 0, 0, 0 }; // Selection rectangle
double xmin = -2.25, xmax = 0.75, ymin = -1.5, ymax = 1.5; // Original Mandelbrot coordinates

void AddMenus(HWND);
void DrawMandelbrot(HWND);
void UpdateMandelbrot(HWND);
void DrawSelectionRect(HWND);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const LPCWSTR CLASS_NAME = L"GTWin32";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"GTWin32", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800, NULL, NULL, hInstance, NULL);

	if (!hwnd) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	AddMenus(hwnd);
	ShowWindow(hwnd, nCmdShow);

	// Set initial dimensions
	RECT rect;
	GetClientRect(hwnd, &rect);
	prevWidth = rect.right;
	prevHeight = rect.bottom;

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void AddMenus(HWND hwnd) {
	HMENU hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hDrawMenu = CreateMenu();
	HMENU hHelpMenu = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, 1, L"Exit");
	AppendMenu(hDrawMenu, MF_STRING, 2, L"Reset Image");
	AppendMenu(hHelpMenu, MF_STRING, 3, L"About");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDrawMenu, L"Draw");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"?");

	SetMenu(hwnd, hMenu);
}

void DrawMandelbrot(HWND hwnd) {
	const int max_iter = 2000;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right;
	int height = rect.bottom;

	for (int py = 0; py < height; ++py) {
		double y0 = ymin + (ymax - ymin) * py / height;
		for (int px = 0; px < width; ++px) {
			double x0 = xmin + (xmax - xmin) * px / width;

			double x = 0.0;
			double y = 0.0;
			int iteration = 0;

			while (x * x + y * y <= 4.0 && iteration < max_iter) {
				double xtemp = x * x - y * y + x0;
				y = 2 * x * y + y0;
				x = xtemp;
				++iteration;
			}

			COLORREF color = (iteration == max_iter) ? RGB(0, 0, 0) : RGB(255 - (iteration * 10) % 255, 255 - (iteration * 5) % 255, 255);
			SetPixel(hdc, px, py, color);
		}
	}

	EndPaint(hwnd, &ps);

	// Generate a beep to indicate the calculation is complete
	Beep(750, 300); // Frequency 750 Hz, duration 300 ms
}

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

void UpdateMandelbrot(HWND hwnd) {
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

void ResetMandelbrot(HWND hwnd) {
	// Reset to initial coordinates
	xmin = -2.25;
	xmax = 0.75;
	ymin = -1.5;
	ymax = 1.5;

	// Force redraw
	InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == 1) {
			PostQuitMessage(0);
		}        
		else if (LOWORD(wParam) == 2) { // Reset
			ResetMandelbrot(hwnd);
		}
		else {
			MessageBox(hwnd, L"GTWin32\n\nA simple Mandelbrot set viewer\n(c)Copyright 2025 by Gennaro E. Tangari", L"About", MB_OK);
		}
		break;
	case WM_ENTERSIZEMOVE:
		isResizing = true;
		break;
	case WM_EXITSIZEMOVE:
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
	case WM_LBUTTONDOWN:
		isSelecting = true;
		startPoint.x = LOWORD(lParam);
		startPoint.y = HIWORD(lParam);
		selectionRect.left = startPoint.x;
		selectionRect.top = startPoint.y;
		selectionRect.right = startPoint.x;
		selectionRect.bottom = startPoint.y;
		break;
	case WM_MOUSEMOVE:
		if (isSelecting) {
			DrawSelectionRect(hwnd); // Erase the previous rectangle
			endPoint.x = LOWORD(lParam);
			endPoint.y = HIWORD(lParam);
			selectionRect.right = endPoint.x;
			selectionRect.bottom = endPoint.y;
			DrawSelectionRect(hwnd); // Draw the new rectangle
		}
		break;
	case WM_LBUTTONUP:
		if (isSelecting) {
			isSelecting = false;
			DrawSelectionRect(hwnd); // Erase the rectangle
			UpdateMandelbrot(hwnd);
		}
		break;
	case WM_PAINT:
		if (!isResizing) { // Avoid drawing during resizing or moving
			DrawMandelbrot(hwnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

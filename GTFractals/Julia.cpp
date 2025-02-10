#include "Julia.h"

// Function to draw the Julia set
void DrawJulia(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

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

	// Calculate the height of the available area
	int height = rect.bottom - toolbarHeight - statusBarHeight;

	isJulia = true;

	for (int py_screen = 0; py_screen < height; ++py_screen) {
		double y0 = ymin + (ymax - ymin) * py_screen / height;
		for (int px_screen = 0; px_screen < width; ++px_screen) {
			double x0 = xmin + (xmax - xmin) * px_screen / width;

			double x = x0;
			double y = y0;
			int iteration = 0;

			while (x * x + y * y <= 4.0 && iteration < max_iter) {
				double xtemp = x * x - y * y + px;
				y = 2 * x * y + py;
				x = xtemp;
				++iteration;
			}

			COLORREF color;
			switch (selectedColorPattern) {
			case 0:
				color = GetModernColor(iteration, max_iter);
				break;
			case 1:
				color = GetDarkGradientColor(iteration, max_iter);
				break;
			case 2:
				color = GetVintageColor(iteration, max_iter);
				break;
			default:
				color = GetModernColor(iteration, max_iter);
				break;
			}
			
			SetPixel(hdc, px_screen, py_screen + toolbarHeight, color);			
		}
	}

	EndPaint(hwnd, &ps);
}
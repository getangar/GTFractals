#include "Mandelbrot.h"

// Function to draw the Mandelbrot set
void DrawMandelbrot(HWND hwnd) {
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

	int progress = 0;

	for (int py = 0; py < height; ++py) {
		double y0 = ymin + (ymax - ymin) * py / height;
		for (int px = 0; px < width; ++px) {

			if (py % 10 == 0) {
				progress = (py * 100) / height;
				SendMessage(hProgressBar, PBM_SETPOS, progress, 0);
			}

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

			// Draw below the toolbar and above the status bar
			SetPixel(hdc, px, py + toolbarHeight, color);
		}
	}

	SendMessage(hProgressBar, PBM_SETPOS, 100, 0);
	EndPaint(hwnd, &ps);
}

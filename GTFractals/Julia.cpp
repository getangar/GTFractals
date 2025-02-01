#include "Julia.h"

// Function to draw the Julia set
void DrawJulia(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right;
	int height = rect.bottom;

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

			COLORREF color = (iteration == max_iter) ? RGB(0, 0, 0) : RGB(255 - (iteration * 10) % 255, 255 - (iteration * 5) % 255, 255);
			SetPixel(hdc, px_screen, py_screen, color);
		}
	}

	EndPaint(hwnd, &ps);
}
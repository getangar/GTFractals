#include "Mandelbrot.h"
#include "colors.h"

// Function to draw the Mandelbrot set
void DrawMandelbrot(HWND hwnd) {
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

			COLORREF color;
			switch (selectedColorPattern) {
			case 0:
				color = GetModernColor(iteration, max_iter);
				break;
			case 1:
				color = GetDargGradientColor(iteration, max_iter);
				break;
			case 2:
				color = GetVintageColor(iteration, max_iter);
				break;
			default:
				color = GetModernColor(iteration, max_iter);
				break;
			}

			SetPixel(hdc, px, py, color);
		}
	}

	EndPaint(hwnd, &ps);
}
#include "export.h"

// Save the current fractal as a bitmap image
BOOL SaveBitmap(HWND hwnd, LPCWSTR filename) {
	HDC hdcScreen = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right;
	int height = rect.bottom;

	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
	SelectObject(hdcMem, hbmScreen);

	BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

	BITMAP bmp;
	GetObject(hbmScreen, sizeof(BITMAP), &bmp);

	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmp.bmWidth;
	bi.biHeight = -bmp.bmHeight; // Negative to avoid vertical flipping
	bi.biPlanes = 1;
	bi.biBitCount = 24; // 24-bit color
	bi.biCompression = BI_RGB;
	bi.biSizeImage = ((bmp.bmWidth * 3 + 3) & ~3) * bmp.bmHeight;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = bi.biSizeImage;
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char* lpbitmap = (char*)GlobalLock(hDIB);

	GetDIBits(hdcScreen, hbmScreen, 0, (UINT)bmp.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdcScreen);
		DeleteObject(hbmScreen);
		return FALSE;
	}

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfType = 0x4D42;
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	DWORD dwWritten;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwWritten, NULL);

	CloseHandle(hFile);
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcScreen);
	DeleteObject(hbmScreen);

	return TRUE;
}

// Print the current fractal
BOOL PrintFractal(HWND hwnd) {
	PRINTDLG pd = { 0 };
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner = hwnd;
	pd.Flags = PD_RETURNDC;

	if (!PrintDlg(&pd)) {
		MessageBox(hwnd, L"User canceled print.", L"Error", MB_ICONERROR); // User canceled print
		return FALSE;
	}

	HDC hdcPrinter = pd.hDC;
	if (!hdcPrinter) {
		MessageBox(hwnd, L"Failed to get printer DC.", L"Error", MB_ICONERROR);		
		return FALSE;
	}

	DOCINFO di = { 0 };
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = L"GTFractals Print";

	if (StartDoc(hdcPrinter, &di) > 0) {
		StartPage(hdcPrinter);

		// Get the client area excluding the UI elements
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);

		// Adjust the height to exclude toolbar and status bar
		int toolbarHeight = 0, statusBarHeight = 0;

		// Get the height of the toolbar
		if (hToolBar) {
			RECT rcTool;
			GetWindowRect(hToolBar, &rcTool);
			toolbarHeight = rcTool.bottom - rcTool.top;
		}

		// Get the height of the status bar
		if (hStatusBar) {
			RECT rcStatus;
			GetWindowRect(hStatusBar, &rcStatus);
			statusBarHeight = rcStatus.bottom - rcStatus.top;
		}

		int fractalWidth = clientRect.right;
		int fractalHeight = clientRect.bottom - toolbarHeight - statusBarHeight;

		// Create a compatible DC to copy only the fractal area
		HDC hdcScreen = GetDC(hwnd);
		HDC hdcMem = CreateCompatibleDC(hdcScreen);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, fractalWidth, fractalHeight);
		SelectObject(hdcMem, hbmMem);

		// Copy only the fractal area (excluding UI)
		BitBlt(hdcMem, 0, 0, fractalWidth, fractalHeight, hdcScreen, 0, toolbarHeight, SRCCOPY);

		// Print the fractal bitmap
		StretchBlt(hdcPrinter, 100, 100, 500, 500, hdcMem, 0, 0, fractalWidth, fractalHeight, SRCCOPY);

		// Cleanup
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdcScreen);

		EndPage(hdcPrinter);
		EndDoc(hdcPrinter);
	}

	DeleteDC(hdcPrinter);    
	return TRUE;
}
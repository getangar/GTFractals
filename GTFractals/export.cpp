#include "export.h"

// Save the current fractal as a bitmap image
BOOL SaveBitmap(HWND hwnd, LPCWSTR filePath) {
	// Get the client area of the window
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// Adjust for UI elements (Toolbar & Status Bar)
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

	// Get the device context of the window
	HDC hdcScreen = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, fractalWidth, fractalHeight);
	SelectObject(hdcMem, hbmMem);

	// Copy only the fractal drawing area
	BitBlt(hdcMem, 0, 0, fractalWidth, fractalHeight, hdcScreen, 0, toolbarHeight, SRCCOPY);

	// Save the bitmap to file
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = fractalWidth;
	bi.biHeight = -fractalHeight; // Negative height for top-down bitmap
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((fractalWidth * bi.biBitCount + 31) / 32) * 4 * fractalHeight;
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char* lpbitmap = (char*)GlobalLock(hDIB);

	GetDIBits(hdcScreen, hbmMem, 0, fractalHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	HANDLE hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(hwnd, L"Failed to create file!", L"Error", MB_ICONERROR);
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdcScreen);
		return false;
	}

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfType = 0x4D42; // BM
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	DWORD dwBytesWritten;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	CloseHandle(hFile);
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcScreen);

	return true;
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
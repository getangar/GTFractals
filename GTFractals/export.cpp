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
	pd.Flags = PD_RETURNDC | PD_USEDEVMODECOPIESANDCOLLATE;
	pd.nCopies = 1;

	if (!PrintDlg(&pd)) {
		return FALSE; // User cancelled the print dialog
	}

	HDC hdcPrinter = pd.hDC;
	if (!hdcPrinter) {
		MessageBox(hwnd, L"Failed to initialize printer.", L"Print Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Start the print job
	DOCINFO di = { 0 };
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = L"GTFractals Print";

	if (StartDoc(hdcPrinter, &di) > 0) {
		if (StartPage(hdcPrinter) > 0) {
			HDC hdcScreen = GetDC(hwnd);
			HDC hdcMem = CreateCompatibleDC(hdcScreen);

			RECT rect;
			GetClientRect(hwnd, &rect);
			int width = rect.right;
			int height = rect.bottom;

			HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
			SelectObject(hdcMem, hbmScreen);

			BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

			// Stretch and print the fractal on paper
			StretchBlt(hdcPrinter, 100, 100, width * 3, height * 3, hdcMem, 0, 0, width, height, SRCCOPY);

			EndPage(hdcPrinter);
		}
		EndDoc(hdcPrinter);
	}

	DeleteDC(hdcPrinter);
	return TRUE;
}
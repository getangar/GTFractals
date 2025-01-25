#include<windows.h>

void AddMenus(HWND);
void AddControls(HWND);

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

	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"GTWin32", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

	if (!hwnd) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	AddMenus(hwnd);
	AddControls(hwnd);
	ShowWindow(hwnd, nCmdShow);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
};

void AddMenus(HWND hwnd) {
	HMENU hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, 1, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

	SetMenu(hwnd, hMenu);
}

void AddControls(HWND hwnd) {
	CreateWindowW(L"button", L"Click Me", WS_VISIBLE | WS_CHILD, 10, 10, 100, 30, hwnd, (HMENU)2, NULL, NULL);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND:
			if (LOWORD(wParam) == 1) {
				PostQuitMessage(0);
			}
			else if (LOWORD(wParam) == 2) {
				MessageBox(hwnd, L"Button Clicked!", L"Message", MB_OK);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			const char* text = "Hello, World!";
			TextOutA(hdc, 10, 10, text, strlen(text));

			EndPaint(hwnd, &ps);
		}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}
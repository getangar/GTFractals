#include<windows.h>
#include<stdio.h>

// Variabili globali per il ridimensionamento
bool isResizing = false;
int prevWidth = 0, prevHeight = 0;

void AddMenus(HWND);
void AddControls(HWND);
void DrawMandelbrot(HWND);

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

    // Imposta le dimensioni iniziali
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
    HMENU hHelpMenu = CreateMenu();

    AppendMenu(hFileMenu, MF_STRING, 4, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 1, L"Exit");
    AppendMenu(hHelpMenu, MF_STRING, 3, L"About");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"?");

    SetMenu(hwnd, hMenu);
}

void AddControls(HWND hwnd) {
    CreateWindowW(L"button", L"Click Me", WS_VISIBLE | WS_CHILD, 10, 10, 100, 30, hwnd, (HMENU)2, NULL, NULL);
}

void DrawMandelbrot(HWND hwnd) {
    const double xmin = -2.25, xmax = 0.75, ymin = -1.5, ymax = 1.5;
    const int max_iter = 250;

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

            COLORREF color = (iteration == max_iter) ? RGB(0, 0, 0) : RGB(255, 255, 255);
            SetPixel(hdc, px, py, color);
        }
    }

    EndPaint(hwnd, &ps);
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
        else if (LOWORD(wParam) == 3) {
            MessageBox(hwnd, L"(c)Copyright 2025 by Gennaro Eduardo Tangari.", L"About", MB_OK);
        }
        break;
    case WM_ENTERSIZEMOVE:
        isResizing = true;
        break;
    case WM_EXITSIZEMOVE:
        isResizing = false;

        // Controlla se le dimensioni sono cambiate
        RECT rect;
        GetClientRect(hwnd, &rect);
        if (rect.right != prevWidth || rect.bottom != prevHeight) {
            prevWidth = rect.right;
            prevHeight = rect.bottom;
            InvalidateRect(hwnd, NULL, TRUE); // Forza il ridisegno
        }
        break;
    case WM_PAINT:
        if (!isResizing) {
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

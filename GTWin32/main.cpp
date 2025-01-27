#include<windows.h>
#include<stdio.h>

// Variabili globali
bool isSelecting = false; // Indica se è in corso la selezione
POINT startPoint, endPoint; // Coordinate di inizio e fine selezione
RECT selectionRect = { 0, 0, 0, 0 }; // Rettangolo di selezione
double xmin = -2.25, xmax = 0.75, ymin = -1.5, ymax = 1.5; // Coordinate Mandelbrot originali

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

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"GTWin32", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    AddMenus(hwnd);
    ShowWindow(hwnd, nCmdShow);

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

    AppendMenu(hFileMenu, MF_STRING, 1, L"Exit");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

    SetMenu(hwnd, hMenu);
}

void DrawMandelbrot(HWND hwnd) {
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

            COLORREF color = (iteration == max_iter) ? RGB(0, 0, 0) : RGB(255 - (iteration * 10) % 255, 255 - (iteration * 5) % 255, 255);
            SetPixel(hdc, px, py, color);
        }
    }

    EndPaint(hwnd, &ps);

    // Genera un beep per indicare che il calcolo è terminato
    Beep(750, 300); // Frequenza 750 Hz, durata 300 ms
}

void DrawSelectionRect(HWND hwnd) {
    HDC hdc = GetDC(hwnd); // Ottieni il contesto grafico

    // Usa la modalità XOR per creare un effetto trasparente
    int oldMode = SetROP2(hdc, R2_XORPEN);

    // Penna con bordo rosso
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Disegna il rettangolo
    Rectangle(hdc, selectionRect.left, selectionRect.top, selectionRect.right, selectionRect.bottom);

    // Ripristina lo stato precedente
    SelectObject(hdc, hOldPen);
    SetROP2(hdc, oldMode);

    DeleteObject(hPen);
    ReleaseDC(hwnd, hdc); // Rilascia il contesto grafico
}


void UpdateMandelbrot(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int width = rect.right;
    int height = rect.bottom;

    // Converti le coordinate dello schermo in coordinate del piano complesso
    double newXmin = xmin + (xmax - xmin) * selectionRect.left / width;
    double newXmax = xmin + (xmax - xmin) * selectionRect.right / width;
    double newYmin = ymin + (ymax - ymin) * selectionRect.top / height;
    double newYmax = ymin + (ymax - ymin) * selectionRect.bottom / height;

    xmin = newXmin;
    xmax = newXmax;
    ymin = newYmin;
    ymax = newYmax;

    // Forza il ridisegno
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            PostQuitMessage(0);
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
            // Cancella il rettangolo precedente
            DrawSelectionRect(hwnd);

            // Aggiorna le coordinate
            endPoint.x = LOWORD(lParam);
            endPoint.y = HIWORD(lParam);
            selectionRect.right = endPoint.x;
            selectionRect.bottom = endPoint.y;

            // Disegna il nuovo rettangolo
            DrawSelectionRect(hwnd);
        }
        break;
    case WM_LBUTTONUP:
        if (isSelecting) {
            isSelecting = false;
            endPoint.x = LOWORD(lParam);
            endPoint.y = HIWORD(lParam);
            selectionRect.right = endPoint.x;
            selectionRect.bottom = endPoint.y;
            UpdateMandelbrot(hwnd);
        }
        break;
    case WM_PAINT:
        DrawMandelbrot(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

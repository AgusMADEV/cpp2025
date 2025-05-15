#include <windows.h>
#include <math.h>

#define TIMER_ID 1
#define PI 3.14159265

// Global variables for animation
int xOffset = 0;
int angle = 0;

// Window procedure function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
        case WM_CREATE:
            // Start the timer
            SetTimer(hwnd, TIMER_ID, 16, NULL); // ~60 FPS
            break;
        case WM_TIMER:
            if (wp == TIMER_ID) {
                angle += 5;
                if (angle >= 360) angle = 0;
                xOffset = (int)(100 * sin(angle * PI / 180)); // Sine wave horizontal motion
                InvalidateRect(hwnd, NULL, TRUE); // Force redraw
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Clear background
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));

            // Set circle position
            int centerX = 250 + xOffset;
            int centerY = 200;
            int radius = 30;

            // Draw circle
            HBRUSH brush = CreateSolidBrush(RGB(0, 128, 255));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
            Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
            SelectObject(hdc, oldBrush);
            DeleteObject(brush);

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ID);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// Main entry point of the program
int main() {
    const char className[] = "MyWindowClass";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProcedure, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL, className, NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, className, "Sine Wave Circle", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, wc.hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(className, wc.hInstance);
    return 0;
}

#include <windows.h>

// Window procedure function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
        case WM_DESTROY:
            PostQuitMessage(0); // Post quit message when the window is closed
            break;
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// Main entry point of the program
int main() {
    // Step 1: Initialize the window class
    const char className[] = "MyWindowClass";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProcedure, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, className, NULL };
    RegisterClassEx(&wc);

    // Step 2: Create the window
    HWND hwnd = CreateWindowEx(0, className, "Basic Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, wc.hInstance, NULL);

    // Step 3: Show and update the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Step 4: Message loop
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Step 5: Unregister class and cleanup
    UnregisterClass(className, wc.hInstance);
    return 0;
}

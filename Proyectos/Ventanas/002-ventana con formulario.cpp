#include <windows.h>
#include <string>

// Global handles for the input fields and button
HWND hInput1, hInput2, hButton;

// Window procedure function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            // Create first input field
            hInput1 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   50, 50, 200, 25, hwnd, NULL, NULL, NULL);

            // Create second input field
            hInput2 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   50, 90, 200, 25, hwnd, NULL, NULL, NULL);

            // Create a button
            hButton = CreateWindow("BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   50, 130, 100, 30, hwnd, (HMENU)1, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wp) == 1) { // Button ID
                char text1[256], text2[256];
                GetWindowText(hInput1, text1, sizeof(text1));
                GetWindowText(hInput2, text2, sizeof(text2));

                // Display a message box with the contents
                MessageBox(hwnd, (std::string("Input 1: ") + text1 + "\nInput 2: " + text2).c_str(), "Submitted Data", MB_OK);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// Main entry point
int main() {
    const char className[] = "MyWindowClass";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProcedure, 0, 0, GetModuleHandle(NULL),
                      NULL, NULL, NULL, NULL, className, NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, className, "Form with Inputs", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 320, 250, NULL, NULL, wc.hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(className, wc.hInstance);
    return 0;
}

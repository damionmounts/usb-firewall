#include <windows.h>
#include <iostream>

// Prototype of window procedure for processing messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Entry point (WinMain ASCII)
// HINSTANCE hPrevInstance skipped
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int showCmd) {

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc; // Pointer to window procedure
    wc.hInstance = hInstance; // Handle to application instance
    const char *CLASS_NAME = "Hiding Window Class"; // Window class name
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc); // Register window class

    // Create window
    HWND hwnd = CreateWindowExA(
            0, // No extended styling
            CLASS_NAME, // Window class name
            "_", // Window name (don't care)
            WS_OVERLAPPEDWINDOW, // No styling (don't care)
            0, // X position (don't care)
            0, // Y position (don't care)
            500, // Width (don't care)
            500, // Height (don't care)
            nullptr, // Has no parent
            nullptr, // No external menu window used
            hInstance, // Instance to associate with window
            nullptr // No creation struct necessary
    );

    // If the window could not be made, GetLastError for more information
    if (hwnd == nullptr) { return -1; }

    RAWINPUTDEVICE devices[2];
    devices[0].usUsagePage = 1;             // Generic desktop controls
    devices[0].usUsage = 6;                 // Keyboard
    devices[0].hwndTarget = hwnd;           // Associated window handle
    devices[0].dwFlags = RIDEV_DEVNOTIFY;   // Message on removal/addition

    devices[1].usUsagePage = 1;             // Generic desktop controls
    devices[1].usUsage = 2;                 // Mouse
    devices[1].hwndTarget = hwnd;           // Associated window handle
    devices[1].dwFlags = RIDEV_DEVNOTIFY;   // Message on removal/addition

    UINT RawInpDevSize = sizeof(RAWINPUTDEVICE);

    // If the raw inputs couldn't be registered, GetLastError for more info
    bool regDevices = RegisterRawInputDevices(devices, 2, RawInpDevSize);
    if (!regDevices) { return -1; }

    //Options: showCmd, SW_RESTORE, SW_HIDE
    ShowWindow(hwnd, SW_HIDE);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    devices[0].dwFlags = RIDEV_REMOVE;  // Remove device
    devices[0].hwndTarget = nullptr;    // No device target window

    devices[1].dwFlags = RIDEV_REMOVE;  // Remove device
    devices[1].hwndTarget = nullptr;    // No device target window

    // If the raw inputs couldn't be unregistered, GetLastError for more info
    bool unregDevices = RegisterRawInputDevices(devices, 2, RawInpDevSize);
    if (!unregDevices) { return -1; }

    return 0;
}

//Window procedure for processing messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // Device addition/removal case
        case WM_INPUT_DEVICE_CHANGE: {
            if (wParam == GIDC_ARRIVAL) {
                std::cout << "Device Added." << std::endl;
                // TODO: Alert | enter quarantine mode
            }
            else { //wParam == GIDC_REMOVAL)
                std::cout << "Device Removed" << std::endl;
                // TODO: Notify of removal | low-priority
            }
            return 0;
        }

        // Window close request case
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        // Window redraw request case
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }

        // Ensure unhandled messages are still processed
        default: {
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
    }
}

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
            0, // No styling (don't care)
            0, // X position (don't care)
            0, // Y position (don't care)
            0, // Width (don't care)
            0, // Height (don't care)
            nullptr, // Has no parent
            nullptr, // No external menu window used
            hInstance, // Instance to associate with window
            nullptr // No creation struct necessary
    );

    // If the window could not be made, GetLastError for more information
    if (hwnd == nullptr) { return -1; }

    RAWINPUTDEVICE devices[1];
    devices[0].usUsagePage = 1; // Generic desktop controls
    devices[0].usUsage = 6; // Keyboard
    devices[0].hwndTarget = hwnd; // Associated window handle
    // Events generated for: any input | device addition/removal
    devices[0].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;

    bool registerKeyboard = RegisterRawInputDevices(
            devices, 1, sizeof(RAWINPUTDEVICE));

    // If the keyboard raw input couldn't be registered, GetLastError for more information
    if (!registerKeyboard) { return -1; }

    //Options: showCmd, SW_RESTORE, SW_HIDE
    ShowWindow(hwnd, SW_HIDE);

    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

//Window procedure for processing messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // Input message case
        case WM_INPUT: {
            UINT dwSize;

            GetRawInputData((HRAWINPUT) lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
            auto lpb = new BYTE[dwSize];

            if (GetRawInputData((HRAWINPUT) lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
                OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
            }

            auto *raw = (RAWINPUT *) lpb;

            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                std::cout << "Extra Info: " << raw->data.keyboard.ExtraInformation << std::endl;
                std::cout << "Flags: " << raw->data.keyboard.Flags << std::endl;
                std::cout << "MakeCode: " << raw->data.keyboard.MakeCode << std::endl;
                std::cout << "Message: " << raw->data.keyboard.Message << std::endl;
                std::cout << "Reserved: " << raw->data.keyboard.Reserved << std::endl;
                std::cout << "VKey: " << raw->data.keyboard.VKey << std::endl;
            }

            unsigned int dataSize = 2048;
            char *deviceName = (char *) malloc(dataSize);
            GetRawInputDeviceInfoA(raw->header.hDevice, RIDI_DEVICENAME, deviceName, &dataSize);
            std::cout << deviceName << std::endl;
            free(deviceName);
            std::cout << std::endl;

            delete[] lpb;
            return 0;
        }

        // Device addition/removal case
        case WM_INPUT_DEVICE_CHANGE: {
            if(wParam == GIDC_ARRIVAL) {
                std::cout << "Device Added." << std::endl;
            }
            else { // wParam == GIDC_REMOVAL
                std::cout << "Device Removed" << std::endl;
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
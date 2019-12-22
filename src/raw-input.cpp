#include <windows.h>
#include <iostream>
#include <winuser.h>

// Prototype of window procedure for processing messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int showCmd) {

    WNDCLASSA wc = {};

    wc.lpfnWndProc = WindowProc; // Pointer to window procedure
    wc.hInstance = hInstance; // Handle to application instance
    const char* CLASS_NAME = "Hiding Window Class"; // Window class name
    wc.lpszClassName = CLASS_NAME;

    // Register window class
    RegisterClass(&wc);

    // Create window
    HWND hwnd = CreateWindowExA(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            "Learn to Program Windows",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            nullptr,       // Parent window
            nullptr,       // Menu
            hInstance,  // Instance handle
            nullptr        // Additional application data
    );

    if (hwnd == nullptr) {
        return 0;
    }

    RAWINPUTDEVICE devices[1];
    devices[0].usUsagePage = 1; //Generic desktop controls
    devices[0].usUsage = 6; //Keyboard
    devices[0].hwndTarget = hwnd; //Ghost window handle
    devices[0].dwFlags = RIDEV_INPUTSINK; //CONSUME

    RegisterRawInputDevices(devices, 1, sizeof(RAWINPUTDEVICE));

    //Options: showCmd, SW_RESTORE, SW_HIDE
    ShowWindow(hwnd, SW_HIDE);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

//Window procedure for processing messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_INPUT:
        {
            UINT dwSize;

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
            auto lpb = new BYTE[dwSize];

            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
                OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
            }

            auto* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                std::cout << "Extra Info: " << raw->data.keyboard.ExtraInformation << std::endl;
                std::cout << "Flags: " << raw->data.keyboard.Flags << std::endl;
                std::cout << "MakeCode: " << raw->data.keyboard.MakeCode << std::endl;
                std::cout << "Message: " << raw->data.keyboard.Message << std::endl;
                std::cout << "Reserved: " << raw->data.keyboard.Reserved << std::endl;
                std::cout << "VKey: " << raw->data.keyboard.VKey << std::endl;
            }

            unsigned int dataSize = 2048;
            char *deviceName = (char*)malloc(dataSize);
            GetRawInputDeviceInfoA(raw->header.hDevice, RIDI_DEVICENAME, deviceName, &dataSize);
            std::cout << deviceName << std::endl;
            free(deviceName);
            std::cout << std::endl;

            delete[] lpb;
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
            return 0;
        }

            // Ensure uncaught messages are still processed
        default: {
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
    }
}
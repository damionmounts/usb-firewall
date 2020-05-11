#include <windows.h>
#include <iostream>

ULONGLONG t_ms_start;
bool mouse_lock = false;
bool keyboard_lock = false;
HHOOK llKeyboardHook, llMouseHook;

// Application Instance & Device Additions / Removals --------------------------

// Prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

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
    HWND window = CreateWindowExA(
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

    //Options: showCmd, SW_RESTORE, SW_HIDE
    ShowWindow(window, showCmd);

    // If the window could not be made, GetLastError for more information
    if (window == nullptr) { return -1; }

    RAWINPUTDEVICE devices[2];
    devices[0].usUsagePage = 1;             // Generic desktop controls
    devices[0].usUsage = 6;                 // Keyboard
    devices[0].hwndTarget = window;           // Associated window handle
    devices[0].dwFlags = RIDEV_DEVNOTIFY;   // Message on removal/addition

    devices[1].usUsagePage = 1;             // Generic desktop controls
    devices[1].usUsage = 2;                 // Mouse
    devices[1].hwndTarget = window;           // Associated window handle
    devices[1].dwFlags = RIDEV_DEVNOTIFY;   // Message on removal/addition

    UINT RawInpDevSize = sizeof(RAWINPUTDEVICE);

    // If the raw inputs couldn't be registered, GetLastError for more info
    bool registerSuccess = RegisterRawInputDevices(devices, 2, RawInpDevSize);
    if (!registerSuccess) { return -1; }

    // If keyboard hooking fails, GetLastError has more information
    llKeyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    if (llKeyboardHook == nullptr) { return -1; }

    // If mouse hooking fails, GetLastError has more information
    llMouseHook = SetWindowsHookExA(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
    if (llMouseHook == nullptr) { return -1; }

    t_ms_start = GetTickCount();

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // If the keyboard hook couldn't be removed, GetLastError has more info
    bool unhookKeyboard = UnhookWindowsHookEx(llKeyboardHook);
    if (!unhookKeyboard) { return -1; }

    // If the mouse hook couldn't be removed, GetLastError has more info
    bool unhookMouse = UnhookWindowsHookEx(llMouseHook);
    if (!unhookMouse) { return -1; }

    devices[0].dwFlags = RIDEV_REMOVE;  // Remove device
    devices[0].hwndTarget = nullptr;    // No device target window

    devices[1].dwFlags = RIDEV_REMOVE;  // Remove device
    devices[1].hwndTarget = nullptr;    // No device target window

    // If the raw inputs couldn't be unregistered, GetLastError for more info
    bool unregisterSuccess = RegisterRawInputDevices(devices, 2, RawInpDevSize);
    if (!unregisterSuccess) { return -1; }

    return 0;
}

//Window procedure for processing messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // Device addition/removal case
        case WM_INPUT_DEVICE_CHANGE: {

            // If 500ms have passed after opening -> actually process additions/removals
            if ((GetTickCount() - t_ms_start) > 500) {
                if (wParam == GIDC_ARRIVAL) {
                    std::cout << "Device Added." << std::endl;
                    Beep(1000, 200);
                    keyboard_lock = true;
                }
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

// Global Hooking Facilities --------------------------

void printKeyData(KBDLLHOOKSTRUCT *keyData) {
    std::cout << "Time: " << keyData->time << std::endl;
    std::cout << "Flags: " << keyData->flags << std::endl;
    std::cout << "Scan Code: " << keyData->scanCode << std::endl;
    std::cout << "Vk Code: " << keyData->vkCode << std::endl;
    std::cout << "Extra Info: " << keyData->dwExtraInfo << std::endl;
    std::cout << std::endl;
}

bool keyDown(WPARAM wParam) {
    return ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN));
}

// Mouse hook procedure
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {

    // nCode < 0: Message must be passed on
    if (nCode < 0) {
        return CallNextHookEx(llMouseHook, nCode, wParam, lParam);
    }

    // Message is processed and blocked from system
    else {
        std::cout << "Mouse event!" << std::endl;

        // ToDo: Add captured input to log

        // FIXME: Mouse is passed-on for testing | should block in release

        // If locked -> don't call next hook
        if (mouse_lock) {
            return -1;
        } else {
            return CallNextHookEx(llMouseHook, nCode, wParam, lParam);
        }
    }
}

// Keyboard hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    // nCode < 0: Message must be passed on
    if (nCode < 0) {
        return CallNextHookEx(llKeyboardHook, nCode, wParam, lParam);
    }

        // Message is processed and blocked from system
    else {
        if (keyDown(wParam)) {
            std::cout << "DOWN" << std::endl;
        } else {
            std::cout << "UP" << std::endl;
        }
        printKeyData((KBDLLHOOKSTRUCT *) lParam);

        // ToDo: Add keyboard state keeping to aid with translation to text
        // ToDo: Add captured input to log

        // If locked -> don't call next hook
        if (keyboard_lock) {
            return -1;
        } else {
            return CallNextHookEx(llKeyboardHook, nCode, wParam, lParam);
        }
    }
}
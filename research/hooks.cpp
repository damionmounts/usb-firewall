#include <iostream>
#include <windows.h>
#include <chrono>

HHOOK llKeyboardHook, llMouseHook;

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

        //return -1;
        //return CallNextHookEx(llMouseHook, nCode, wParam, lParam);
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

        return -1;
        //return CallNextHookEx(llKeyboardHook, nCode, wParam, lParam);
    }
}

// Entry point
// WinMain (ASCII)
// HINSTANCE hPrevInstance skipped
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {

    // If keyboard hooking fails, GetLastError has more information
    llKeyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    if (llKeyboardHook == nullptr) { return -1; }

    // If mouse hooking fails, GetLastError has more information
    llMouseHook = SetWindowsHookExA(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
    if (llMouseHook == nullptr) { return -1; }

    // Message loop
    MSG msg;
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

    return 0;
}

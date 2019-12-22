#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>

HHOOK hHook;

void printKeyData(KBDLLHOOKSTRUCT *keyData) {
    std::cout << "Time: " << keyData->time << std::endl;
    std::cout << "Flags: " << keyData->flags << std::endl;
    std::cout << "Scan Code: " << keyData->scanCode << std::endl;
    std::cout << "Vk Code: " << keyData->vkCode << std::endl;
    std::cout << "Extra Info: " << keyData->dwExtraInfo << std::endl;
    std::cout << std::endl;
}

// Keyboard hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    // If nCode is less than zero, the hook procedure must pass the message to the CallNextHookEx
    if (nCode < 0) {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }

        // Perform actions and pass down the hook chain
        // Return < 0 to block others from reading input
    else {
        printKeyData((KBDLLHOOKSTRUCT *) lParam);

        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hHook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    if (hHook == nullptr) {
        return -1; // Failed to create hook
        // GetLastError has more information
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);

    return 0;
}
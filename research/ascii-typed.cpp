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

// Keyboard hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    // nCode < 0: Message must be passed on
    if (nCode < 0) {
        return CallNextHookEx(llKeyboardHook, nCode, wParam, lParam);
    }

        // Message is processed and blocked from system
    else {
//        if (keyDown(wParam)) {
//            std::cout << "DOWN" << std::endl;
//        } else {
//            std::cout << "UP" << std::endl;
//        }

        KBDLLHOOKSTRUCT* keyEvent = (KBDLLHOOKSTRUCT *) lParam;

        //printKeyData(keyEvent);
        byte keyState[256];
        GetKeyboardState(keyState);

        WORD c;
        LPWORD c_ptr = &c;

        bool menu = GetKeyState(VK_MENU);

        bool p = ToAscii(keyEvent->vkCode, keyEvent->scanCode, keyState, c_ptr, menu);

        //std::cout << p << std::endl;
        if (p && !keyDown(wParam) && c >= 32 && c <= 126)
            std::cout << (char)c;

        return CallNextHookEx(llKeyboardHook, nCode, wParam, lParam);
    }
}

// Entry point
// WinMain (ASCII)
// HINSTANCE hPrevInstance skipped
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {

    // If keyboard hooking fails, GetLastError has more information
    llKeyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    if (llKeyboardHook == nullptr) { return -1; }

    for (int i = 0; i < 256; i++) {

    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // If the keyboard hook couldn't be removed, GetLastError has more info
    bool unhookKeyboard = UnhookWindowsHookEx(llKeyboardHook);
    if (!unhookKeyboard) { return -1; }

    return 0;
}

#include <iostream>
#include <windows.h>
#include <vector>

#include "utils.h"

void printRidiDeviceInfo(RID_DEVICE_INFO *devInfo) {
    std::cout << "Device type: " << devInfo->dwType << std::endl;
    if (devInfo->dwType == 0) {
        std::cout << "[Mouse]" << std::endl;
        std::cout << "ID: " << devInfo->mouse.dwId << std::endl;
        std::cout << "# of Buttons: " << devInfo->mouse.dwNumberOfButtons << std::endl;
        std::cout << "Sample Rate: " << devInfo->mouse.dwSampleRate << std::endl;
        std::cout << "Has Horizontal Scroll: " << devInfo->mouse.fHasHorizontalWheel << std::endl;
    } else if (devInfo->dwType == 1) {
        std::cout << "[Keyboard]" << std::endl;
        std::cout << "Type: " << devInfo->keyboard.dwType << std::endl;
        std::cout << "Sub-type: " << devInfo->keyboard.dwSubType << std::endl;
        std::cout << "Scan code mode : " << devInfo->keyboard.dwKeyboardMode << std::endl;
        std::cout << "# of Function Keys: " << devInfo->keyboard.dwNumberOfFunctionKeys << std::endl;
        std::cout << "# of Indicators: " << devInfo->keyboard.dwNumberOfIndicators << std::endl;
        std::cout << "# of Keys: " << devInfo->keyboard.dwNumberOfKeysTotal << std::endl;
    } else { //devInfo.dwType == 2
        std::cout << "[Non-Mouse | Non-Keyboard]" << std::endl;

        std::cout << std::hex;
        std::cout << "Product ID: 0x" << devInfo->hid.dwProductId << std::endl;
        std::cout << "Vendor ID: 0x" << devInfo->hid.dwVendorId << std::endl;
        std::cout << std::dec;

        std::cout << "Version #: " << devInfo->hid.dwVersionNumber << std::endl;
        std::cout << "Usage: " << devInfo->hid.usUsage << std::endl;
        std::cout << "Usage Page: " << devInfo->hid.usUsagePage << std::endl;
    }
}

int main() {
    UINT numberOfDevices;
    unsigned int ret = GetRawInputDeviceList(nullptr, &numberOfDevices, sizeof(RAWINPUTDEVICELIST));
    std::cout << "Return: " << ret << " # of Devices: " << numberOfDevices << std::endl;

    auto ptrDevices = static_cast<PRAWINPUTDEVICELIST>(malloc(sizeof(RAWINPUTDEVICELIST) * numberOfDevices));

    ret = GetRawInputDeviceList(ptrDevices, &numberOfDevices, sizeof(RAWINPUTDEVICELIST));
    std::cout << "Return: " << ret << std::endl;

    char *deviceName = (char *) malloc(1024);
    unsigned int dataSize;

    for (unsigned int i = 0; i < numberOfDevices; i++) {
        std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
        std::cout << "Device " << i << ": " << std::endl;

        dataSize = 1024;
        GetRawInputDeviceInfoA(ptrDevices[i].hDevice, RIDI_DEVICENAME, deviceName, &dataSize);

        std::string devNameStr = deviceName;
        devNameStr = devNameStr.substr(4); //remove \\?\ /

        std::cout << deviceName << std::endl;

        std::vector<std::string> tokens = splitString(devNameStr, '#');
        for (unsigned int nn = 0; nn < tokens.size(); nn++) {
            std::cout << "tokens[" << nn << "] = " << tokens[nn] << std::endl;
        }

        unsigned long dataBufSize = 8192;
        char *dataBuf = (char *) malloc(dataBufSize);

        std::string subKey = R"(SYSTEM\CurrentControlSet\Enum\)" + tokens[0] + "\\" + tokens[1] + "\\" + tokens[2];

        DWORD type;

        LSTATUS x = RegGetValueA(
                HKEY_LOCAL_MACHINE,
                subKey.c_str(),
                "DeviceDesc",
                RRF_RT_ANY,
                &type,
                dataBuf,
                &dataBufSize
        );

        std::cout << "Field type: " << type << std::endl;
        std::cout << "DeviceDesc: " << dataBuf << std::endl;

        free(dataBuf);

        RID_DEVICE_INFO devInfo;
        devInfo.cbSize = sizeof(RID_DEVICE_INFO);
        dataSize = sizeof(RID_DEVICE_INFO);
        GetRawInputDeviceInfoA(ptrDevices[i].hDevice, RIDI_DEVICEINFO, &devInfo, &dataSize);
        printRidiDeviceInfo(&devInfo);
    }

    free(deviceName);
    free(ptrDevices);

    return 0;
}
#ifndef USBFIREWALL_UTILS_H
#define USBFIREWALL_UTILS_H

#include <string>
#include <vector>

//Returns vector of strings after splitting the input by a delimiter character
std::vector<std::string> splitString(const std::string& str, char delimiter);

#endif //USBFIREWALL_UTILS_H

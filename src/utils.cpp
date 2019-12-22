#include <vector>
#include <string>
#include <sstream>

#include "utils.h"

//Returns vector of strings after splitting the input by a delimiter character
std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

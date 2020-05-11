# CMake Version Requirement
cmake_minimum_required(VERSION 3.15)

# ------------------------------------------------------------------------------
# Entire Solution
project(USBFirewall)        # USBFirewall
set(CMAKE_CXX_STANDARD 20)  # C++ 2020

# ------------------------------------------------------------------------------
# Projects (Research)
add_executable(Raw_Input research/raw-input.cpp research/utils.cpp research/utils.h)
add_executable(Hooks research/hooks.cpp research/utils.cpp research/utils.h)
add_executable(Present research/present-devices.cpp research/utils.cpp research/utils.h)
add_executable(MVP mvp-src/mvp.cpp)
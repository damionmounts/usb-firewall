# CMake Version Requirement
cmake_minimum_required(VERSION 3.15)

# ------------------------------------------------------------------------------
# Entire Solution
project(USBFirewall)        # USBFirewall
set(CMAKE_CXX_STANDARD 20)  # C++ 2020

# ------------------------------------------------------------------------------
# Projects (Research)
add_executable(Present_Devices src/present-devices.cpp src/utils.cpp src/utils.h)
add_executable(Raw_Input src/raw-input.cpp src/utils.cpp src/utils.h)
cmake_minimum_required(VERSION 3.16)
project(Accessory)

set(CMAKE_CXX_STANDARD 11)

add_executable(Accessory AdkEcho.cpp AOA/AOA.cpp AOA/AOA.h)

target_link_libraries(Accessory  usb-1.0 pthread z)
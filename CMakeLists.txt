cmake_minimum_required(VERSION 3.16)
project(accessory_mode)

set(CMAKE_CXX_STANDARD 11)

add_executable(go_accessory_mode accessory_mode.c include/android_open_accessory.c include/android_open_accessory.h include/android_transporter.c include/android_transporter.h)

target_link_libraries(go_accessory_mode  usb-1.0 pthread z)
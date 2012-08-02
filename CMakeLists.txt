project(usb-motor-stepper)
cmake_minimum_required(VERSION 2.8)

# to find some custome cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package(Boost REQUIRED)
find_package(Crossbuild)

add_subdirectory(host)
add_subdirectory(device)


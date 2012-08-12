project(usb-motor-stepper)
cmake_minimum_required(VERSION 2.8)

# to find some custome cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package(Boost REQUIRED)
find_package(Crossbuild)

add_subdirectory(host)
add_subdirectory(device)

set (CPACK_PACKAGE_EXECUTABLES "motor-stepper")
set (CPACK_GENERATOR "ZIP")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0")
#string(REGEX MATCH [0-9]+ CPACK_PACKAGE_VERSION_PATCH "$Revision: 298 $")

include(CPack)

# install bin directory
#install(DIRECTORY ${PROJECT_BINARY_DIR}/bin DESTINATION .)

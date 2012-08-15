# usb-motor-stepper/host/test

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/device/common
	${PROJECT_SOURCE_DIR}/device/sim
	${PROJECT_SOURCE_DIR}/host/lib
)

add_executable(test-usb-motor-stepper
	device_test.cpp
	hello_test.cpp
	serial_test.cpp
	sim_test.cpp
)

if (WIN32)
target_link_libraries(test-usb-motor-stepper ums)
else (WIN32)
target_link_libraries(test-usb-motor-stepper boost_unit_test_framework boost_system boost_thread ums)
endif (WIN32)

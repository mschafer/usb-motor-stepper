# usb-motor-stepper/host/test

add_executable(test-usb-motor-stepper
	hello_test.cpp
	serial_test.cpp
	sim_test.cpp
)

include_directories ( 
    ${Boost_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/device/common
	${PROJECT_SOURCE_DIR}/device/sim
	${PROJECT_SOURCE_DIR}/host/lib
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries(test-usb-motor-stepper boost_unit_test_framework boost_system boost_thread ums)

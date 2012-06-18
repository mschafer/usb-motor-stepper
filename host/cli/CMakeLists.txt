# usb-motor-stepper/host/test

add_executable(stepper
	main.cpp
)

include_directories ( 
    ${Boost_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/device/common
	${PROJECT_SOURCE_DIR}/device/sim
	${PROJECT_SOURCE_DIR}/host/lib
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries(stepper boost_system boost_thread boost_program_options ums)

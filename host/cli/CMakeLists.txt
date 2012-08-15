# usb-motor-stepper/host/cli

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/device/common
	${PROJECT_SOURCE_DIR}/device/sim
	${PROJECT_SOURCE_DIR}/host/lib
)

add_executable(stepper
	main.cpp
)

install (TARGETS stepper
  RUNTIME DESTINATION bin
)
  
if (WIN32)
target_link_libraries(stepper ums)
else (WIN32)
target_link_libraries(stepper boost_system boost_thread boost_program_options ums)
endif (WIN32)


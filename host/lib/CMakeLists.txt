# usb-motor-stepper/host/lib

aux_source_directory(${PROJECT_SOURCE_DIR}/device/common COMMON_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/device/sim SIM_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/host/lib LIB_SRCS)

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( ${Boost_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/device/common
	${PROJECT_SOURCE_DIR}/device/sim
	${PROJECT_SOURCE_DIR}/host/lib
)

add_library (ums SHARED
	${COMMON_SRCS}
	${SIM_SRCS}
	${LIB_SRCS}
)

if ( NOT WIN32 )
target_link_libraries(ums boost_system boost_thread)
endif ( NOT WIN32)

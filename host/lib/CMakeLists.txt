# usb-motor-stepper/host/lib

aux_source_directory(${PROJECT_SOURCE_DIR}/device/common COMMON_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/device/sim SIM_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/host/lib LIB_SRCS)

add_library (ums STATIC
	${COMMON_SRCS}
	${SIM_SRCS}
	${LIB_SRCS}
)

include_directories ( ${Boost_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/device/common
	${PROJECT_SOURCE_DIR}/device/sim
)

link_directories(${Boost_LIBRARY_DIRS})

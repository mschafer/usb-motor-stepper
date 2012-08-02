# device/lpc-p1343-rowley

set (DEV_NAME "lpc-p1343-rowley")
set (DEV_SRC_DIR "${PROJECT_SOURCE_DIR}/device/${DEV_NAME}")
set (DEV_BUILD_DIR "${DEV_SRC_DIR}/THUMB Flash Release" )


add_custom_target ( ${DEV_NAME}_firmware ALL
  COMMAND
  ${CROSSBUILD_EXECUTABLE} -project "ums" -config "THUMB Flash Release"  ${DEV_SRC_DIR}/ums.hzp
  COMMAND
  ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/firmware/${DEV_NAME}
  COMMAND
  ${CMAKE_COMMAND} -E copy ${DEV_BUILD_DIR}/ums.bin ${PROJECT_BINARY_DIR}/firmware/${DEV_NAME}
)

#install(DIRECTORY ${PROJECT_BINARY_DIR}/fcec_image DESTINATION .)

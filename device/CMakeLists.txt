# device directory

if (EXISTS ${CROSSBUILD_EXECUTABLE})
  add_subdirectory(lpc-p1343-rowley)
  message(STATUS "building firmware for lpc-p1343 with Rowley CrossStudio")
endif (EXISTS ${CROSSBUILD_EXECUTABLE})

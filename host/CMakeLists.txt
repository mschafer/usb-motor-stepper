# usb-motor-stepper/host

add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(cli)

# inf file is needed for win32 to recognize virtual com port
if (WIN32)
  install (FILES lpc134x-vcom_64.inf DESTINATION inf)
endif (WIN32)
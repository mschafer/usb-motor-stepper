# This module defines the following variables:
#   CROSSBUILD_EXECUTABLE - path to crossbuild command line client
#   CROSSBUILD_FOUND - true if the command line client was found

find_program(CROSSBUILD_EXECUTABLE
  NAMES
  crossbuild.exe
  PATHS
  "c:/Program Files/Rowley Associates Limited/CrossWorks for ARM 2.0/bin"
  "c:/Program Files (x86)/Rowley Associates Limited/CrossWorks for ARM 2.0/bin"
)

mark_as_advanced(CROSSBUILD_EXECUTABLE)

# Handle the QUIETLY and REQUIRED arguments and set CROSSBUILD_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(crossbuild DEFAULT_MSG CROSSBUILD_EXECUTABLE)

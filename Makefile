
CMAKE ?= /usr/bin/cmake
CMAKE_DEFS = -D CMAKE_BUILD_TYPE:string=Debug -D CMAKE_VERBOSE_MAKEFILE:bool=true 
CMAKE_OPTS = $(CMAKE_DEFS) --debug-output -G "Unix Makefiles"

.PHONY : cmake_generate

# cmake doesn't appear to accept an output directory argument so we need to cd
cmake_generate:
	rm -rf build
	mkdir -p build
	cd build; $(CMAKE) $(CMAKE_OPTS) ..

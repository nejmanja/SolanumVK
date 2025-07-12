set(_sdl2_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${CMAKE_SOURCE_DIR}/include"
)
# put user specified location at beginning of search
if(SDL2_ROOT_DIR)
	set(_sdl2_HEADER_SEARCH_DIRS "${SDL2_ROOT_DIR}"
	"${SDL2_ROOT_DIR}/include"
	${_sdl2_HEADER_SEARCH_DIRS})
endif(SDL2_ROOT_DIR)

if(NOT SDL2_LIB_ROOT_DIR)
    set(SDL2_LIB_ROOT_DIR "${LIB_ROOT_DIR}")
endif(NOT SDL2_LIB_ROOT_DIR)

# locate header
find_path(SDL2_INCLUDE_DIR "SDL2/sdl.h"
    PATHS ${_sdl2_HEADER_SEARCH_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_INCLUDE_DIR)

find_library(SDL2_LIBRARY
    NAMES sdl2
    PATHS "${SDL2_LIB_ROOT_DIR}/${CMAKE_BUILD_TYPE}")

find_file(SDL2_DLL
    NAMES sdl2.dll
    PATHS "${SDL2_LIB_ROOT_DIR}/${CMAKE_BUILD_TYPE}")

if(NOT SDL2_LIBRARY)
    message("Couldn't find SDL2 lib!")
    set(SDL2_FOUND False)
endif(NOT SDL2_LIBRARY)

if(SDL2_FOUND)
	set(SDL2_INCLUDE_DIRS "${SDL2_INCLUDE_DIR}")
	message(STATUS "SDL2_INCLUDE_DIR = ${SDL2_INCLUDE_DIR}")
	message(STATUS "SDL2_LIBRARY = ${SDL2_LIBRARY}")
    # copy the SDL2 dll into the main project, so that SDL doesn't complain
    file(COPY ${SDL2_DLL} DESTINATION ${CMAKE_BINARY_DIR})
endif(SDL2_FOUND)
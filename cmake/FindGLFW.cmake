set(_glfw_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${CMAKE_SOURCE_DIR}/include"
)
# put user specified location at beginning of search
if(GLFW_ROOT_DIR)
	set(_glfw_HEADER_SEARCH_DIRS "${GLFW_ROOT_DIR}"
	"${GLFW_ROOT_DIR}/include"
	${_glfw_HEADER_SEARCH_DIRS})
endif(GLFW_ROOT_DIR)

if(NOT GLFW_LIB_ROOT_DIR)
    set(GLFW_LIB_ROOT_DIR "${LIB_ROOT_DIR}")
endif(NOT GLFW_LIB_ROOT_DIR)

# locate header
find_path(GLFW_INCLUDE_DIR "GLFW/glfw3.h"
    PATHS ${_glfw_HEADER_SEARCH_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_INCLUDE_DIR)

find_library(GLFW_LIBRARY
    NAMES glfw3
    PATHS "${GLFW_LIB_ROOT_DIR}/${CMAKE_BUILD_TYPE}")

if(NOT GLFW_LIBRARY)
    message("Couldn't find GLFW lib!")
    set(GLFW_FOUND False)
endif(NOT GLFW_LIBRARY)

if(GLFW_FOUND)
	set(GLFW_INCLUDE_DIRS "${GLFW_INCLUDE_DIR}")
	message(STATUS "GLFW_INCLUDE_DIR = ${GLFW_INCLUDE_DIR}")
	message(STATUS "GLFW_LIBRARY = ${GLFW_LIBRARY}")
endif(GLFW_FOUND)
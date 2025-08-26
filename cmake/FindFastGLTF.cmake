set(_fastGltf_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${CMAKE_SOURCE_DIR}/include"
)
# put user specified location at beginning of search
if(FastGLTF_ROOT_DIR)
	set(_fastGltf_HEADER_SEARCH_DIRS "${FastGLTF_ROOT_DIR}"
	"${FastGLTF_ROOT_DIR}/include"
	${_fastGltf_HEADER_SEARCH_DIRS})
endif(FastGLTF_ROOT_DIR)

if(NOT FastGLTF_LIB_ROOT_DIR)
    set(FastGLTF_LIB_ROOT_DIR "${LIB_ROOT_DIR}")
endif(NOT FastGLTF_LIB_ROOT_DIR)

# locate header
find_path(FastGLTF_INCLUDE_DIR "FastGLTF/core.hpp"
    PATHS ${_fastGltf_HEADER_SEARCH_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FastGLTF DEFAULT_MSG FastGLTF_INCLUDE_DIR)

find_library(FastGLTF_LIBRARY
    NAMES fastgltf
    PATHS "${FastGLTF_LIB_ROOT_DIR}/${CMAKE_BUILD_TYPE}")

if(NOT FastGLTF_LIBRARY)
    message("Couldn't find FastGLTF lib!")
    set(FastGLTF_FOUND False)
endif(NOT FastGLTF_LIBRARY)

if(FastGLTF_FOUND)
	set(FastGLTF_INCLUDE_DIRS "${FastGLTF_INCLUDE_DIR}")
	message(STATUS "FastGLTF_INCLUDE_DIR = ${FastGLTF_INCLUDE_DIR}")
	message(STATUS "FastGLTF_LIBRARY = ${FastGLTF_LIBRARY}")
endif(FastGLTF_FOUND)
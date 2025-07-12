set(_vkBootstrap_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${CMAKE_SOURCE_DIR}/include"
)
# put user specified location at beginning of search
if(SDL2_ROOT_DIR)
	set(_vkBootstrap_HEADER_SEARCH_DIRS "${SDL2_ROOT_DIR}"
	"${SDL2_ROOT_DIR}/include"
	${_vkBootstrap_HEADER_SEARCH_DIRS})
endif(SDL2_ROOT_DIR)

if(NOT VKB_LIB_ROOT_DIR)
    set(VKB_LIB_ROOT_DIR "${LIB_ROOT_DIR}")
endif(NOT VKB_LIB_ROOT_DIR)

# locate header
find_path(VkBootstrap_INCLUDE_DIR "vk-bootstrap/VkBootstrap.h"
    PATHS ${_vkBootstrap_HEADER_SEARCH_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VkBootstrap DEFAULT_MSG VkBootstrap_INCLUDE_DIR)

find_library(VkBootstrap_LIBRARY
    NAMES "vk-bootstrap"
    PATHS "${VKB_LIB_ROOT_DIR}/${CMAKE_BUILD_TYPE}")

if(NOT VkBootstrap_LIBRARY)
    message("Couldn't find VkBootstrap lib!")
    set(VkBootstrap_FOUND False)
endif(NOT VkBootstrap_LIBRARY)

if(VkBootstrap_FOUND)
	set(VkBootstrap_INCLUDE_DIRS "${VkBootstrap_INCLUDE_DIR}")
	message(STATUS "VkBootstrap_INCLUDE_DIR = ${VkBootstrap_INCLUDE_DIR}")
	message(STATUS "VkBootstrap_LIBRARY = ${VkBootstrap_LIBRARY}")
endif(VkBootstrap_FOUND)
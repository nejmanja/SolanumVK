cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(SolanumVK VERSION 1.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2 REQUIRED)
find_package(VkBootstrap REQUIRED)

include_directories(
    ${GLM_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}/SDL2 # so that ImGui won't complain...
    ${VkBootstrap_INCLUDE_DIR}
)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/headers/*.h
	${CMAKE_SOURCE_DIR}/headers/*.hpp
    ${CMAKE_SOURCE_DIR}/imgui/*.h)

file(GLOB IMGUI_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/imgui/*.cpp
    ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_sdl2.cpp)
    
    
include_directories(${CMAKE_SOURCE_DIR}/headers)
include_directories(${CMAKE_SOURCE_DIR}/headers/renderers)
include_directories(${CMAKE_SOURCE_DIR}/headers/pipelines)

include_directories(${CMAKE_SOURCE_DIR}/imgui)


add_executable(SolanumVK ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARY} ${SDL2_LIBRARY} ${VkBootstrap_LIBRARY})

if (WIN32)
    add_custom_command(
        TARGET SolanumVK POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${SDL2_DLL}" "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
        VERBATIM
    )

    add_custom_command(
        TARGET SolanumVK POST_BUILD
        COMMAND "${CMAKE_SOURCE_DIR}/compile_shaders.bat"
    )
endif()
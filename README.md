# SolanumVK
Personal learning and experimenting platform. Doubles as an engine that aims to abstract over Vulkan in a plug-and-play lego-blocks-like fashion.

# Compiling and Running
This project uses CMake as a build system. It depends on Vulkan, SDL2 and GLFW for windowing (both implementations exist), GLM for math, FastGLTF for mesh loading, and ImGui. CMake Find scripts are included, and additional search paths may be provided by specifying `<LIB_NAME>_ROOT_DIR`.

Currently, only Windows is actually supported, as it is the OS used for development. There's a batch script for bulk-compiling shaders located inside the `shaders` folder.

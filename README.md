# SolanumVK
Personal platform for learning and experimenting with Vulkan. Doubles as an engine that aims to abstract over Vulkan in a plug-and-play lego-blocks-like fashion.

# Compiling and Running
This project uses CMake as a build system. It depends on Vulkan, SDL2 for windowing, GLM for math, and ImGui. CMake Find scripts are included, and additional search paths may be provided by specifying `<LIB_NAME>_ROOT_DIR`. Check the provided `settings.json` for an example usage inside of VSCode.

Currently, only Windows is actually supported, as it is the OS used for development. There's a batch script for bulk-compiling shaders located inside the `shaders` folder.

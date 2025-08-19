#include "WindowBridgeGLFW.h"

#include <GLFW/glfw3.h>
#include <vector>

#if _DEBUG
constexpr bool useValidationLayers = true;
#else
constexpr bool useValidationLayers = false;
#endif

WindowBridgeGLFW::WindowBridgeGLFW(bool resizeable)
{
    uint32_t glfwExtensionCount;
    const char **glfwExtensions;
    // get the extensions required by GLFW for interfacing the windowing system
    // these are always required!
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (useValidationLayers)
        // just for avoiding typos - literally "VK_EXT_debug_utils"
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    instanceExtensions = extensions;
}

WindowBridgeGLFW::~WindowBridgeGLFW()
{
}

VkSurfaceKHR WindowBridgeGLFW::createSurface(VkInstance instance)
{
    return VkSurfaceKHR{};
}

void WindowBridgeGLFW::handleEvents()
{
}
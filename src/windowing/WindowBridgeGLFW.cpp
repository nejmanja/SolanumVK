#include "WindowBridgeGLFW.h"

#include <vector>

#include "SolanumConstants.h"
#include "VulkanUtils.h"

#if _DEBUG
constexpr bool useValidationLayers = true;
#else
constexpr bool useValidationLayers = false;
#endif

WindowBridgeGLFW::WindowBridgeGLFW(const bool resizeable) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, resizeable);
    window = glfwCreateWindow(SolVK::windowWidth, SolVK::windowHeight, SolVK::AppName.c_str(), nullptr, nullptr);
    if (!glfwVulkanSupported()) {
        printf("GLFW: Vulkan Not Supported\n");
        abort();
    }

    uint32_t glfwExtensionCount;
    // get the extensions required by GLFW for interfacing the windowing system
    // these are always required!
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (useValidationLayers)
        // just for avoiding typos - literally "VK_EXT_debug_utils"
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    instanceExtensions = extensions;

    // GLFW can't comprehend the `this` parameter in callbacks
    // hence the need for the user pointer to 'this' instance.
    glfwSetWindowUserPointer(window, this);
    // The callback itself is static, and the class will be retrieved inside of it
    glfwSetKeyCallback(window, keyCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Also yoinked from example, main function
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // there's also gamepad input lol
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    auto mainScale = ImGui_ImplGlfw_GetContentScaleForWindow(window); // ImGui_ImplSDL2_GetContentScaleForDisplay(0);
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;

    ImGui_ImplGlfw_InitForVulkan(window, true);
}

WindowBridgeGLFW::~WindowBridgeGLFW() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

VkSurfaceKHR WindowBridgeGLFW::createSurface(VkInstance instance) {
    // Create Window Surface
    VkSurfaceKHR surface;
    const VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    VulkanUtils::CheckVkResult(err);

    return surface;
}

void WindowBridgeGLFW::handleEvents() {
    glfwPollEvents();
}

void WindowBridgeGLFW::keyCallback(GLFWwindow *window, const int key, const int scancode, const int action,
                                   const int mods) {
    auto *windowBridge = static_cast<WindowBridgeGLFW *>(glfwGetWindowUserPointer(window));
    if (windowBridge == nullptr)
        return;

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_W:
                windowBridge->lastKeyPress = KeyCode::KeyW;
                break;
            case GLFW_KEY_A:
                windowBridge->lastKeyPress = KeyCode::KeyA;
                break;
            case GLFW_KEY_S:
                windowBridge->lastKeyPress = KeyCode::KeyS;
                break;
            case GLFW_KEY_D:
                windowBridge->lastKeyPress = KeyCode::KeyD;
                break;
            case GLFW_KEY_Q:
                windowBridge->lastKeyPress = KeyCode::KeyQ;
                break;
            case GLFW_KEY_E:
                windowBridge->lastKeyPress = KeyCode::KeyE;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, 1);
                break;
            default:
                break;
        }
    } else if (action == GLFW_RELEASE) {
        windowBridge->lastKeyPress = KeyCode::None;
    }
}

void WindowBridgeGLFW::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    auto *windowBridge = static_cast<WindowBridgeGLFW *>(glfwGetWindowUserPointer(window));
    if (windowBridge == nullptr)
        return;

    windowBridge->lastMouseOffset = windowBridge->currentMouseOffset;

    windowBridge->currentMouseOffset = MouseOffset{static_cast<float>(xpos), static_cast<float>(ypos)};

    windowBridge->lastMouseMovementFrameIndex = windowBridge->frameIndex;
}

void WindowBridgeGLFW::scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    auto *windowBridge = static_cast<WindowBridgeGLFW *>(glfwGetWindowUserPointer(window));
    if (windowBridge == nullptr)
        return;

    windowBridge->currentScrollOffset = ScrollOffset{static_cast<float>(xOffset), static_cast<float>(yOffset)};

    windowBridge->lastScrollFrameIndex = windowBridge->frameIndex;
}

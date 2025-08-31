#pragma once

#include "IWindowBridge.h"

#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"

class WindowBridgeGLFW : public IWindowBridge
{
public:
    WindowBridgeGLFW(bool resizeable);
    ~WindowBridgeGLFW();

    VkSurfaceKHR createSurface(VkInstance instance) override;
    void handleEvents() override;

    VkExtent2D getExtent() override { return windowExtent; }

    double getTime() override { return currentTime; }
    double getDeltaTime() override { return currentTime - lastFrameTime; }

    bool quitRequested() override { return glfwWindowShouldClose(window); }
    bool isHidden() override { return minimized; }
    void newFrame() override
    {
        lastFrameTime = currentTime;
        currentTime = glfwGetTime();
        ImGui_ImplGlfw_NewFrame();
    }

    std::vector<const char *> getWindowInstanceExtensions() override { return instanceExtensions; };

private:
    double currentTime{0.0}, lastFrameTime{0.0};

    std::vector<const char *> instanceExtensions{};

    VkExtent2D windowExtent;

    GLFWwindow *window;

    bool wantsQuit;
    bool minimized = false;
};
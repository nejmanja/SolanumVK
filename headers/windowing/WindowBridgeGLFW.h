#pragma once

#include "IWindowBridge.h"

#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"

class WindowBridgeGLFW : public IWindowBridge {
public:
    explicit WindowBridgeGLFW(bool resizeable);

    ~WindowBridgeGLFW() override;

    VkSurfaceKHR createSurface(VkInstance instance) override;

    void handleEvents() override;

    VkExtent2D getExtent() override { return windowExtent; }

    double getTime() override { return currentTime; }
    double getDeltaTime() override { return currentTime - lastFrameTime; }

    bool quitRequested() override { return glfwWindowShouldClose(window); }
    bool isHidden() override { return minimized; }

    void newFrame() override {
        frameIndex++;
        lastFrameTime = currentTime;
        currentTime = glfwGetTime();
        ImGui_ImplGlfw_NewFrame();
    }

    [[nodiscard]] KeyCode getLastKeyPress() const override { return lastKeyPress; }

    [[nodiscard]] MouseOffset getMouseOffset() const override {
        return frameIndex - lastMouseMovementFrameIndex > 1
                   ? MouseOffset{}
                   : MouseOffset{currentMouseOffset.x - lastMouseOffset.x, currentMouseOffset.y - lastMouseOffset.y};
    }

    [[nodiscard]] ScrollOffset getScrollOffset() const override {
        return frameIndex - lastScrollFrameIndex > 1
                   ? ScrollOffset{}
                   : currentScrollOffset;
    }

    std::vector<const char *> getWindowInstanceExtensions() override { return instanceExtensions; };

private:
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

    ScrollOffset currentScrollOffset{};
    size_t lastScrollFrameIndex{0};
    MouseOffset currentMouseOffset{}, lastMouseOffset{};
    size_t lastMouseMovementFrameIndex{0};
    KeyCode lastKeyPress{KeyCode::None};
    double currentTime{0.0}, lastFrameTime{0.0};

    std::vector<const char *> instanceExtensions{};

    VkExtent2D windowExtent{0, 0};

    GLFWwindow *window;
    bool minimized = false;

    size_t frameIndex{0};
};

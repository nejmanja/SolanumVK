#pragma once

#include "IWindowBridge.h"

class WindowBridgeGLFW : IWindowBridge
{
public:
    WindowBridgeGLFW(bool resizeable);
    ~WindowBridgeGLFW();

    VkSurfaceKHR createSurface(VkInstance instance);
    void handleEvents();

    VkExtent2D getExtent() { return windowExtent; }

    bool quitRequested() { return wantsQuit; }
    bool isHidden() { return minimized; }

    std::vector<const char *> getWindowInstanceExtensions() { return instanceExtensions; };

private:
    std::vector<const char *> instanceExtensions{};

    VkExtent2D windowExtent;

    bool wantsQuit;
    bool minimized;
};
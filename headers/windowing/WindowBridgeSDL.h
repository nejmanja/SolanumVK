#pragma once

#include "IWindowBridge.h"

class WindowBridgeSDL : public IWindowBridge
{
public:
    WindowBridgeSDL(bool resizeable);
    ~WindowBridgeSDL();

    VkSurfaceKHR createSurface(VkInstance instance);
    void handleEvents();

    VkExtent2D getExtent() { return windowExtent; }

    bool quitRequested() { return wantsQuit; }
    bool isHidden() { return minimized; }

private:
    VkExtent2D windowExtent;

    struct SDL_Window *window{nullptr};
    bool wantsQuit = false;
    bool minimized = false;
};
#pragma once

#include "IWindowBridge.h"

#include "backends/imgui_impl_sdl2.h"

class WindowBridgeSDL : public IWindowBridge
{
public:
    WindowBridgeSDL(bool resizeable);
    ~WindowBridgeSDL();

    VkSurfaceKHR createSurface(VkInstance instance);
    void handleEvents();

    VkExtent2D getExtent() { return windowExtent; }

    double getTime() override { return currentTime; }
    double getDeltaTime() override { return currentTime - lastFrameTime; }

    bool quitRequested() { return wantsQuit; }
    bool isHidden() { return minimized; }
    void newFrame() override;

    std::vector<const char *> getWindowInstanceExtensions() { return {}; }

private:
    double currentTime{0.0}, lastFrameTime{0.0};

    VkExtent2D windowExtent;

    struct SDL_Window *window{nullptr};
    bool wantsQuit = false;
    bool minimized = false;
};
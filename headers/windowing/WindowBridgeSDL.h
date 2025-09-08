#pragma once

#include "IWindowBridge.h"

#include "backends/imgui_impl_sdl2.h"

class WindowBridgeSDL : public IWindowBridge
{
public:
    WindowBridgeSDL(bool resizeable);
    ~WindowBridgeSDL();

    VkSurfaceKHR createSurface(VkInstance instance) override;
    void handleEvents() override;

    VkExtent2D getExtent() override { return windowExtent; }

    double getTime() override { return currentTime; }
    double getDeltaTime() override { return currentTime - lastFrameTime; }

    bool quitRequested() override { return wantsQuit; }
    bool isHidden() override { return minimized; }
    void newFrame() override;

    const KeyCode getLastKeyPress() const override { return lastKeyPress; }
    const MouseOffset getMouseOffset() const override { return MouseOffset{}; }
    const ScrollOffset getScrollOffset() const override { return ScrollOffset{}; }

    std::vector<const char *> getWindowInstanceExtensions() override { return {}; }

private:
    void processKeyPress(SDL_Event &e);
    KeyCode lastKeyPress;

    double currentTime{0.0}, lastFrameTime{0.0};

    VkExtent2D windowExtent;

    struct SDL_Window *window{nullptr};
    bool wantsQuit = false;
    bool minimized = false;
};
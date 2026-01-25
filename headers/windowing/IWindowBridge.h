#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "InputEnums.h"

class IWindowBridge {
public:
    IWindowBridge() = default;

    virtual ~IWindowBridge() = default;

    virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;

    virtual void handleEvents() = 0;

    virtual VkExtent2D getExtent() = 0;

    virtual double getTime() = 0;

    virtual double getDeltaTime() = 0;

    virtual bool quitRequested() = 0;

    virtual bool isHidden() = 0;

    virtual void newFrame() = 0;

    [[nodiscard]] virtual KeyCode getLastKeyPress() const = 0;

    [[nodiscard]] virtual MouseOffset getMouseOffset() const = 0;

    [[nodiscard]] virtual ScrollOffset getScrollOffset() const = 0;

    virtual std::vector<const char *> getWindowInstanceExtensions() = 0;
};

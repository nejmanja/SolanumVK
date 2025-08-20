#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class IWindowBridge
{
public:
	IWindowBridge() {};
	virtual ~IWindowBridge() {};

	virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;
	virtual void handleEvents() = 0;

	virtual VkExtent2D getExtent() = 0;

	virtual bool quitRequested() = 0;
	virtual bool isHidden() = 0;
	virtual void newFrame() = 0;

	virtual std::vector<const char *> getWindowInstanceExtensions() = 0;
};

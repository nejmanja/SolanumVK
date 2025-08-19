#pragma once

#include <vulkan/vulkan.h>

class IWindowBridge
{
public:
	IWindowBridge() {};
	virtual ~IWindowBridge() {};

	virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;
	virtual void handleEvents() = 0;

	virtual VkExtent2D getExtent() = 0; // { return windowExtent; }

	virtual bool quitRequested() = 0; // { return wantsQuit; }
	virtual bool isHidden() = 0;	  // { return minimized; }
};

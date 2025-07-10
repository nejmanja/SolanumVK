#pragma once

#include <vulkan/vulkan.h>
#include "VkBootstrap/VkBootstrap.h"

#include "WindowBridge.h"

class VulkanContext
{
public:
	VulkanContext(WindowBridge& window);
	~VulkanContext();
private:
	vkb::Instance createInstance();
	void createDevice(vkb::Instance vkbInstance);

	VkSurfaceKHR surface;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;

	VkDebugUtilsMessengerEXT debugMessenger;
};


#pragma once

#include <vulkan/vulkan.h>
#include "VkBootstrap/VkBootstrap.h"

#include "WindowBridge.h"
#include "Swapchain.h"

class VulkanContext
{
public:
	VulkanContext(WindowBridge& window);
	~VulkanContext();
private:
	vkb::Instance createInstance();
	void createDevice(vkb::Instance vkbInstance);
	void createSwapchain(VkExtent2D windowExtent);

	VkSurfaceKHR surface;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;

	Swapchain swapchain;

	VkDebugUtilsMessengerEXT debugMessenger;
};


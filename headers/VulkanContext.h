#pragma once

#include <exception>

#include <vulkan/vulkan.h>
#include <vk-bootstrap/VkBootstrap.h>

#include "WindowBridge.h"
#include "Swapchain.h"

class VulkanContext
{
public:
	enum class QueueType
	{
		Graphics,
		Compute
	};

	VulkanContext(WindowBridge &window);
	~VulkanContext();

	Swapchain &getSwapchain() { return swapchain; }
	VkDevice getDevice() { return device; }
	uint32_t getQueueFamily(QueueType type);
	VkQueue getQueue(QueueType type);

private:
	vkb::Instance createInstance();
	void createDevice(vkb::Instance vkbInstance);
	void createSwapchain(VkExtent2D windowExtent);

	VkSurfaceKHR surface;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;

	uint32_t computeFamily;
	VkQueue computeQueue;
	uint32_t graphicsFamily;
	VkQueue graphicsQueue;

	Swapchain swapchain;

	VkDebugUtilsMessengerEXT debugMessenger;
};

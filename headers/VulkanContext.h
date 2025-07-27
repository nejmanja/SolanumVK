#pragma once

#include <exception>

#include <vma/vk_mem_alloc.h>
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

	VmaAllocator &getVmaAllocator() { return vmaAllocator; }
	Swapchain &getSwapchain() { return swapchain; }
	VkDevice getDevice() { return device; }
	VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
	uint32_t getQueueFamily(QueueType type);
	VkQueue getQueue(QueueType type);

private:
	vkb::Instance createInstance();
	void createDevice(vkb::Instance vkbInstance);
	void createSwapchain(VkExtent2D windowExtent);
	void createVmaAllocator();

	VkSurfaceKHR surface;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;

	VmaAllocator vmaAllocator;

	uint32_t computeFamily;
	VkQueue computeQueue;
	uint32_t graphicsFamily;
	VkQueue graphicsQueue;

	Swapchain swapchain;

	VkDebugUtilsMessengerEXT debugMessenger;
};

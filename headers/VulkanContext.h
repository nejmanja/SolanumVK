#pragma once

#include <exception>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vk-bootstrap/VkBootstrap.h>

#include "IWindowBridge.h"
#include "Swapchain.h"

class VulkanContext
{
public:
	enum class QueueType
	{
		Graphics,
		Compute
	};

	VulkanContext(IWindowBridge &window);
	~VulkanContext();

	const VkInstance &getInstance() const { return instance; }
	const VmaAllocator &getVmaAllocator() const { return vmaAllocator; }
	const Swapchain &getSwapchain() const { return swapchain; }
	const VkDevice getDevice() const { return device; }
	const VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
	const uint32_t getQueueFamily(QueueType type) const;
	const VkQueue getQueue(QueueType type) const;

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

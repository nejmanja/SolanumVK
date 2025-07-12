#pragma once

#include <vulkan/vulkan.h>

#include <vector>

struct Swapchain
{
	VkSwapchainKHR swapchain;
	VkExtent2D extent;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	uint32_t framesInFlight;
};
#pragma once

#include <vulkan/vulkan.h>

struct CommandBuffer
{
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
};
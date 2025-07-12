#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "CommandBuffer.h"

class CommandManager
{
public:
	CommandManager(VkDevice device, uint32_t queueFamily, uint32_t count);
	~CommandManager();

	VkCommandBuffer get(uint32_t index) { return buffers[index].commandBuffer; }
	void begin(uint32_t cmdIndex);
	void transitionImage(uint32_t cmdIndex, VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout);

private:
	VkDevice device;
	std::vector<CommandBuffer> buffers;
};

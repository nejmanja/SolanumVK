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
private:
	VkDevice device;
	std::vector<CommandBuffer> buffers;
};


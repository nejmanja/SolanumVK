#include "CommandManager.h"

CommandManager::CommandManager(VkDevice device, uint32_t queueFamily, uint32_t count)
	: device(device)
{
	VkCommandPoolCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .pNext = nullptr };
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = queueFamily;

	VkCommandBufferAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .pNext = nullptr };
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	for(uint32_t i = 0; i < count; ++i)
	{
		CommandBuffer buffer{};
		vkCreateCommandPool(device, &createInfo, nullptr, &buffer.commandPool);
		allocInfo.commandPool = buffer.commandPool;
		vkAllocateCommandBuffers(device, &allocInfo, &buffer.commandBuffer);
		buffers.push_back(buffer);
	}
}

CommandManager::~CommandManager()
{
	// Also frees all command buffers
	for (auto& buffer : buffers)
	{
		vkDestroyCommandPool(device, buffer.commandPool, nullptr);
	}
}

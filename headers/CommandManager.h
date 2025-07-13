#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "CommandBuffer.h"

class CommandManager
{
public:
	CommandManager(VkDevice device, uint32_t queueFamily, uint32_t count);
	~CommandManager();

	VkCommandBuffer get() { return buffers[frameIdx].commandBuffer; }
	void begin();
	void reset() { vkResetCommandBuffer(get(), 0); }
	void end() { vkEndCommandBuffer(get()); }
	void transitionImage(VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout);
	void nextFrame() { frameIdx = (frameIdx + 1) % buffers.size(); }

	void submit(VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence signalFence);
	void clearImage(VkImage image, VkClearColorValue clearValue);

private:
	VkDevice device;
	std::vector<CommandBuffer> buffers;
	uint32_t frameIdx{0};
};

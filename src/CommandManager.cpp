#include "CommandManager.h"

CommandManager::CommandManager(VkDevice device, uint32_t queueFamily, uint32_t count)
	: device(device)
{
	VkCommandPoolCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .pNext = nullptr};
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = queueFamily;

	VkCommandBufferAllocateInfo allocInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .pNext = nullptr};
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	for (uint32_t i = 0; i < count; ++i)
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
	for (auto &buffer : buffers)
	{
		vkDestroyCommandPool(device, buffer.commandPool, nullptr);
	}
}

void CommandManager::begin()
{
	// begin cmd buffer
	VkCommandBufferBeginInfo beginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .pNext = nullptr};
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(get(), &beginInfo);
}

void CommandManager::transitionImage(VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout)
{
	VkImageMemoryBarrier2 imageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr};
	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = srcLayout;
	imageBarrier.newLayout = dstLayout;

	VkImageAspectFlags aspectMask = (dstLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	imageBarrier.subresourceRange = subresourceRange;
	imageBarrier.image = image;

	VkDependencyInfo depInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .pNext = nullptr};
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(get(), &depInfo);
}

void CommandManager::submit(VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence signalFence)
{
	VkCommandBufferSubmitInfo cmdSubmitInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO, .pNext = nullptr};
	cmdSubmitInfo.commandBuffer = get();
	cmdSubmitInfo.deviceMask = 0;
	VkSemaphoreSubmitInfo semaphoreWaitInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, .pNext = nullptr};
	semaphoreWaitInfo.deviceIndex = 0;
	semaphoreWaitInfo.semaphore = waitSemaphore;
	semaphoreWaitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	semaphoreWaitInfo.value = 1;
	VkSemaphoreSubmitInfo semaphoreSignalInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, .pNext = nullptr};
	semaphoreSignalInfo.deviceIndex = 0;
	semaphoreSignalInfo.semaphore = signalSemaphore;
	semaphoreSignalInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	semaphoreSignalInfo.value = 1;

	VkSubmitInfo2 submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2, .pNext = nullptr};
	submitInfo.flags = 0;
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmdSubmitInfo;

	submitInfo.waitSemaphoreInfoCount = 1;
	submitInfo.pWaitSemaphoreInfos = &semaphoreWaitInfo;
	submitInfo.signalSemaphoreInfoCount = 1;
	submitInfo.pSignalSemaphoreInfos = &semaphoreSignalInfo;

	vkQueueSubmit2(queue, 1, &submitInfo, signalFence);
}

void CommandManager::clearImage(VkImage image, VkClearColorValue clearValue)
{
	VkImageSubresourceRange clearRange{};
	clearRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	clearRange.baseArrayLayer = 0;
	clearRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	clearRange.baseMipLevel = 0;
	clearRange.levelCount = VK_REMAINING_MIP_LEVELS;

	vkCmdClearColorImage(get(), image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}

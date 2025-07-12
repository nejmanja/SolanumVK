#include "RenderingEngine.h"

#include <thread>
#include <cmath>

RenderingEngine::RenderingEngine()
	: window(false),
	  vulkanContext(window),
	  commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics), vulkanContext.getSwapchain().framesInFlight),
	  syncManager(vulkanContext.getDevice(), vulkanContext.getSwapchain().framesInFlight)
{
}

void RenderingEngine::exec()
{
	while (!window.quitRequested())
	{
		window.handleEvents();

		if (window.isHidden())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		draw();
		++frameCounter;
	}
}

RenderingEngine::~RenderingEngine()
{
	// Wait for device to finish all the commands before command buffers get deallocated!
	vkDeviceWaitIdle(vulkanContext.getDevice());
}

void RenderingEngine::draw()
{
	auto frameIdx = getFrameIndex();
	auto device = vulkanContext.getDevice();
	auto renderFence = syncManager.getRenderFence(frameIdx);
	// Wait for previous render to finish
	vkWaitForFences(device, 1, &renderFence, VK_TRUE, UINT64_MAX);
	// Reset the render fence, we're beginning to render a new frame
	vkResetFences(device, 1, &renderFence);

	// Get the swapchain image index. While the range of this and frameIdx are the same,
	// they're not neccesarily in sync (i.e. frameIdx may be 0, and swapchainIdx may be 1)
	// Everything other than the swapchain indices is indexed via frameIdx!
	uint32_t swapchainImageIndex;
	vkAcquireNextImageKHR(
		device,
		vulkanContext.getSwapchain().swapchain,
		UINT64_MAX,
		// Signal the present semaphore, so that drawing commands know that an image was acquired.
		syncManager.getSwapchainImageAcuqiredSemaphore(getFrameIndex()),
		// No fences needed to signal, the CPU waits for the render commands to get issued.
		nullptr,
		&swapchainImageIndex);

	auto cmd = commandManager.get(frameIdx);
	vkResetCommandBuffer(cmd, 0);

	commandManager.begin(frameIdx);
	auto image = vulkanContext.getSwapchain().images[swapchainImageIndex];
	commandManager.transitionImage(frameIdx, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(frameCounter / 120.f));
	clearValue = {{0.0f, 0.0f, flash, 1.0f}};

	VkImageSubresourceRange clearRange{};
	clearRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	clearRange.baseArrayLayer = 0;
	clearRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	clearRange.baseMipLevel = 0;
	clearRange.levelCount = VK_REMAINING_MIP_LEVELS;

	vkCmdClearColorImage(cmd, image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	commandManager.transitionImage(frameIdx, image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	vkEndCommandBuffer(cmd);

	// submit command buffer
	VkCommandBufferSubmitInfo cmdSubmitInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO, .pNext = nullptr};
	cmdSubmitInfo.commandBuffer = cmd;
	cmdSubmitInfo.deviceMask = 0;
	VkSemaphoreSubmitInfo semaphoreWaitInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, .pNext = nullptr};
	semaphoreWaitInfo.deviceIndex = 0;
	semaphoreWaitInfo.semaphore = syncManager.getSwapchainImageAcuqiredSemaphore(frameIdx);
	semaphoreWaitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	semaphoreWaitInfo.value = 1;
	VkSemaphoreSubmitInfo semaphoreSignalInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, .pNext = nullptr};
	semaphoreSignalInfo.deviceIndex = 0;
	semaphoreSignalInfo.semaphore = syncManager.getRenderSemaphore(frameIdx);
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

	vkQueueSubmit2(vulkanContext.getQueue(VulkanContext::QueueType::Graphics), 1, &submitInfo, renderFence);

	// present to screen
	VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, .pNext = nullptr};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkanContext.getSwapchain().swapchain;
	auto renderSemaphore = syncManager.getRenderSemaphore(frameIdx);
	presentInfo.pWaitSemaphores = &renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pImageIndices = &swapchainImageIndex;

	vkQueuePresentKHR(vulkanContext.getQueue(VulkanContext::QueueType::Graphics), &presentInfo);
}

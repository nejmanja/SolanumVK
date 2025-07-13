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
		commandManager.nextFrame();
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
	auto renderSemaphore = syncManager.getRenderSemaphore(frameIdx);
	auto swapchainImageAcquiredSemaphore = syncManager.getSwapchainImageAcuqiredSemaphore(frameIdx);

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
		swapchainImageAcquiredSemaphore,
		// No fences needed to signal, the CPU waits for the render commands to get issued.
		nullptr,
		&swapchainImageIndex);

	commandManager.reset();
	commandManager.begin();
	auto image = vulkanContext.getSwapchain().images[swapchainImageIndex];
	commandManager.transitionImage(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(frameCounter / 120.f));
	clearValue = {{0.0f, 0.0f, flash, 1.0f}};

	commandManager.clearImage(image, clearValue);

	commandManager.transitionImage(image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	commandManager.end();

	commandManager.submit(
		vulkanContext.getQueue(VulkanContext::QueueType::Graphics),
		swapchainImageAcquiredSemaphore,
		renderSemaphore,
		renderFence);

	// present to screen
	VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, .pNext = nullptr};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkanContext.getSwapchain().swapchain;
	presentInfo.pWaitSemaphores = &renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pImageIndices = &swapchainImageIndex;

	vkQueuePresentKHR(vulkanContext.getQueue(VulkanContext::QueueType::Graphics), &presentInfo);
}

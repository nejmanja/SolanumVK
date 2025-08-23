#include "RenderingEngine.h"

#include "WindowBridgeSDL.h"
#include "WindowBridgeGLFW.h"

#include <thread>
#include <cmath>
#include <cstdio>

RenderingEngine::RenderingEngine()
	: window(std::make_unique<WindowBridgeGLFW>(false)),
	  vulkanContext(*window),
	  commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics), vulkanContext.getSwapchain().framesInFlight),
	  syncManager(vulkanContext.getDevice(), vulkanContext.getSwapchain().framesInFlight),
	  renderTarget(CreateRenderTarget(vulkanContext)),
	  renderer(std::make_unique<ComputeRenderer>(vulkanContext, renderTarget.resource)),
	  imGuiRenderer(std::make_unique<ImGuiRenderer>(vulkanContext))
{
}

void RenderingEngine::exec()
{
	while (!window->quitRequested())
	{
		window->newFrame();

		window->handleEvents();

		if (window->isHidden())
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

	ImageAllocator::freeImage(vulkanContext, renderTarget);
}

void RenderingEngine::draw()
{
	auto device = vulkanContext.getDevice();

	// Get the swapchain image index.
	auto swapchainImageIndex = getSwapchainImageIndex(device);

	auto renderFence = syncManager.getRenderFence(swapchainImageIndex);
	auto renderSemaphore = syncManager.getRenderSemaphore(swapchainImageIndex);
	auto swapchainImageAcquiredSemaphore = syncManager.getSwapchainImageAcuqiredSemaphore(swapchainImageIndex);

	// Wait for previous render to finish
	vkWaitForFences(device, 1, &renderFence, VK_TRUE, UINT64_MAX);
	// Reset the render fence, we're beginning to render a new frame
	vkResetFences(device, 1, &renderFence);

	auto swapchainImage = vulkanContext.getSwapchain().images[swapchainImageIndex];
	auto swapchainImageView = vulkanContext.getSwapchain().imageViews[swapchainImageIndex];

	renderer->setup(renderTarget.resource);
	auto swapchainExtent = vulkanContext.getSwapchain().extent;
	imGuiRenderer->setup({.image = swapchainImage, .imageView = swapchainImageView, .imageExtent = {swapchainExtent.width, swapchainExtent.height, 1}});

	// ===============================================================================================================
	// Begin Command Recording
	// ===============================================================================================================
	commandManager.reset();

	commandManager.begin();
	commandManager.transitionImage(renderTarget.resource.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	renderer->execute(commandManager.get());

	commandManager.transitionImage(renderTarget.resource.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	commandManager.transitionImage(swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	commandManager.copyImage(renderTarget.resource.image, swapchainImage, swapchainExtent, swapchainExtent);

	commandManager.transitionImage(swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	imGuiRenderer->execute(commandManager.get());

	commandManager.transitionImage(swapchainImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	commandManager.end();

	// ===============================================================================================================
	// End Command Recording
	// ===============================================================================================================

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

uint32_t RenderingEngine::getSwapchainImageIndex(VkDevice device)
{
	auto swapchainImageAcquiredSemaphore = syncManager.getRecycledSemaphore();
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

	auto oldImageAcquiredSemaphore = syncManager.getSwapchainImageAcuqiredSemaphore(swapchainImageIndex);
	if (oldImageAcquiredSemaphore != VK_NULL_HANDLE)
	{
		syncManager.emplaceRecycledSemaphore(oldImageAcquiredSemaphore);
	}
	syncManager.setSwapchainImageAcuqiredSemaphore(swapchainImageIndex, swapchainImageAcquiredSemaphore);

	return swapchainImageIndex;
}

AllocatedImageResource RenderingEngine::CreateRenderTarget(const VulkanContext &vulkanContext)
{

	VkImageUsageFlags usageFlags{};
	// blitting from image to swapchain
	usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; // todo check if i really need this
	// for Compute shader writes
	usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
	usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto swapchainExtent = vulkanContext.getSwapchain().extent;

	return ImageAllocator::allocateImage2D(vulkanContext,
										   VK_FORMAT_R16G16B16A16_SFLOAT,
										   usageFlags,
										   {swapchainExtent.width, swapchainExtent.height, 1},
										   VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), // gpu-only
										   VMA_MEMORY_USAGE_GPU_ONLY);
}

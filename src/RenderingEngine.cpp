#include "RenderingEngine.h"

#include <thread>
#include <cmath>

RenderingEngine::RenderingEngine()
	: window(false),
	  vulkanContext(window),
	  commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics), vulkanContext.getSwapchain().framesInFlight),
	  syncManager(vulkanContext.getDevice(), vulkanContext.getSwapchain().framesInFlight),
	  renderTarget(CreateRenderTarget(vulkanContext)),
	  renderer(std::make_unique<ComputeRenderer>(vulkanContext, renderTarget.resource)),
	  imGuiRenderer(std::make_unique<ImGuiRenderer>(vulkanContext))
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

	ImageAllocator::freeImage(vulkanContext, renderTarget);
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

	auto swapchainImage = vulkanContext.getSwapchain().images[swapchainImageIndex];
	auto swapchainImageView = vulkanContext.getSwapchain().imageViews[swapchainImageIndex];

	commandManager.reset();

	renderer->setup(renderTarget.resource);
	auto swapchainExtent = vulkanContext.getSwapchain().extent;
	imGuiRenderer->setup({.image = swapchainImage, .imageView = swapchainImageView, .imageExtent = {swapchainExtent.width, swapchainExtent.height, 1}});

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

#include "RenderingEngine.h"

#include <thread>
#include <cmath>

RenderingEngine::RenderingEngine()
	: window(false),
	  vulkanContext(window),
	  commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics), vulkanContext.getSwapchain().framesInFlight),
	  syncManager(vulkanContext.getDevice(), vulkanContext.getSwapchain().framesInFlight),
	  descriptorSetAllocator(vulkanContext.getDevice(), std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{
															{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}})
{
	descriptorLayoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
	auto device = vulkanContext.getDevice();

	descriptorSetLayout = descriptorLayoutBuilder.build(device, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	computePipeline = std::make_unique<ComputePipeline>(device, descriptorSetLayout);

	auto framesInFlight = vulkanContext.getSwapchain().framesInFlight;
	VkWriteDescriptorSet *descriptorWrites = new VkWriteDescriptorSet[framesInFlight];
	VkDescriptorImageInfo *imageInfos = new VkDescriptorImageInfo[framesInFlight];
	for (int i = 0; i < framesInFlight; i++)
	{
		descriptorSets.push_back(descriptorSetAllocator.allocate(descriptorSetLayout));

		imageInfos[i] = {.sampler = nullptr,
						 .imageView = vulkanContext.getSwapchain().imageViews[i],
						 .imageLayout = VK_IMAGE_LAYOUT_GENERAL};

		descriptorWrites[i] = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .pNext = nullptr};
		descriptorWrites[i].descriptorCount = 1;
		descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[i].dstBinding = 0;
		descriptorWrites[i].dstSet = descriptorSets[i];
		descriptorWrites[i].pImageInfo = &imageInfos[i];
	}

	vkUpdateDescriptorSets(device, framesInFlight, descriptorWrites, 0, nullptr);
	delete[] (descriptorWrites);
	delete[] (imageInfos);
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

	vkDestroyDescriptorSetLayout(vulkanContext.getDevice(), descriptorSetLayout, nullptr);
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

	computePipeline->bind(commandManager.get());
	computePipeline->bindDescriptorSets(1, &descriptorSets[swapchainImageIndex]);
	computePipeline->execute();

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

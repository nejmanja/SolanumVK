#include "RenderingEngine.h"

#include "WindowBridgeSDL.h"
#include "WindowBridgeGLFW.h"

#include "DescriptorWriter.h"

#include <thread>
#include <cmath>

RenderingEngine::RenderingEngine()
	: window(std::make_unique<WindowBridgeGLFW>(false)),
	  vulkanContext(*window),
	  commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics), vulkanContext.getSwapchain().framesInFlight),
	  syncManager(vulkanContext.getDevice(), vulkanContext.getSwapchain().framesInFlight),
	  renderTarget(createRenderTarget(vulkanContext)),
	  renderer(std::make_unique<ComputeRenderer>(vulkanContext, renderTarget.resource)),
	  imGuiRenderer(std::make_unique<ImGuiRenderer>(vulkanContext)),
	  memoryManager(vulkanContext)
{
	camera.setPerspectiveProj(glm::radians(45.0f), 1.3333f, 0.01f, 1000.0f);
	// camera.setOrthoProj(1.0f, 0.75f, 0.01f, 1000.0f);

	createSceneDescriptor();
	simpleMeshRenderer = std::make_unique<SimpleMeshRenderer>(vulkanContext, sceneDescriptorLayout, sceneDescriptorSet);
}

void RenderingEngine::exec()
{
	while (!window->quitRequested())
	{
		window->newFrame();

		window->handleEvents();

		processInput();

		if (window->isHidden())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		draw(window->getDeltaTime());

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

void RenderingEngine::draw(double deltaTime)
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

	renderer->setup(renderTarget.resource, deltaTime);
	auto swapchainExtent = vulkanContext.getSwapchain().extent;
	imGuiRenderer->setup({.image = swapchainImage, .imageView = swapchainImageView, .imageExtent = {swapchainExtent.width, swapchainExtent.height, 1}}, deltaTime);
	simpleMeshRenderer->setup(renderTarget.resource, deltaTime);
	// ===============================================================================================================
	// Begin Command Recording
	// ===============================================================================================================
	commandManager.reset();

	commandManager.begin();
	commandManager.transitionImage(renderTarget.resource.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	renderer->execute(commandManager.get());

	commandManager.transitionImage(renderTarget.resource.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	simpleMeshRenderer->execute(commandManager.get());

	commandManager.transitionImage(renderTarget.resource.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
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

void RenderingEngine::processInput()
{
	glm::vec3 offset{0.0f};

	switch (window->getLastKeyPress())
	{
	case KeyCode::KeyS:
		offset.z = -1.0f;
		break;
	case KeyCode::KeyW:
		offset.z = 1.0f;
		break;
	case KeyCode::KeyA:
		offset.x = -1.0f;
		break;
	case KeyCode::KeyD:
		offset.x = 1.0f;
		break;
	case KeyCode::KeyE:
		offset.y = -1.0f;
		break;
	case KeyCode::KeyQ:
		offset.y = 1.0f;
		break;
	}

	auto dt = (float)window->getDeltaTime();
	camera.move(offset * dt);

	auto mouseOffset = window->getMouseOffset();
	camera.rotate(mouseOffset.x * dt, mouseOffset.y * dt);

	auto scrollOffset = window->getScrollOffset();
	if(scrollOffset.y != 0.0f)
		camera.changeSpeed(scrollOffset.y);

	sceneDescriptor.view = camera.getView();
	BufferAllocator::copyBufferData(vulkanContext, &sceneDescriptor, sizeof(SceneDescriptor), sceneUniformBuffer);
}

void RenderingEngine::createSceneDescriptor()
{
	DescriptorLayoutBuilder layoutBuilder{};
	layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
	sceneDescriptorLayout = layoutBuilder.build(vulkanContext.getDevice(), VK_SHADER_STAGE_VERTEX_BIT, 0);
	memoryManager.registerResource(sceneDescriptorLayout);

	auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}};

	sceneDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(vulkanContext.getDevice(), resourceSizes);
	sceneDescriptorSet = sceneDescriptorAllocator->allocate(sceneDescriptorLayout);

	sceneUniformBuffer = BufferAllocator::allocateBuffer(vulkanContext, sizeof(SceneDescriptor), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
	memoryManager.registerResource(sceneUniformBuffer);
	sceneDescriptor = {
		.view = camera.getView(),
		.proj = camera.getProj(),
		.sunDirection = glm::normalize(glm::vec3{0.0f, -1.0f, -1.0f}),
		.sunColor = glm::vec3{1.0f, 0.9f, 0.55f}};
	BufferAllocator::copyBufferData(vulkanContext, &sceneDescriptor, sizeof(SceneDescriptor), sceneUniformBuffer);

	DescriptorWriter::writeBuffer(vulkanContext, sceneDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, sceneUniformBuffer.buffer, sizeof(SceneDescriptor));
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

AllocatedImageResource RenderingEngine::createRenderTarget(const VulkanContext &vulkanContext)
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

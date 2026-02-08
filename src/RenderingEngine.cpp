#include "RenderingEngine.h"

#include "WindowBridgeSDL.h"
#include "WindowBridgeGLFW.h"

#include "DescriptorWriter.h"

#include <thread>

#include "BufferAllocator.h"
#include "ComputeRenderer.h"
#include "DescriptorLayoutBuilder.h"
#include "PBRMeshRenderer.h"
#include "SimpleMeshRenderer.h"
#include "SolanumConstants.h"

RenderingEngine::RenderingEngine()
    : window(std::make_unique<WindowBridgeGLFW>(false)),
      vulkanContext(*window),
      commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics),
                     SolVK::numFramesInFlight),
      syncManager(vulkanContext.getDevice(), SolVK::numFramesInFlight, vulkanContext.getSwapchain().numberOfImages),
      renderTarget(createRenderTarget(vulkanContext)),
      memoryManager(vulkanContext),
      currentSwapchainTarget(vulkanContext.getSwapchainImages()[0]) {
    camera.setPerspectiveProj(glm::radians(45.0f), 1.3333f, 0.01f, 1000.0f);

    createSceneDescriptor();
}

void RenderingEngine::initialize() {
    pbrMeshRenderer = std::make_unique<PBRMeshRenderer>(vulkanContext);

    std::unique_ptr<SimpleRenderer> simpleMeshRenderer = std::make_unique<SimpleMeshRenderer>(
        vulkanContext, sceneDescriptorLayout,
        sceneDescriptorSet);
    std::unique_ptr<SimpleRenderer> computeRenderer = std::make_unique<ComputeRenderer>(vulkanContext, &camera);
    std::unique_ptr<ImageEffectRenderer> imageEffectRenderer = std::make_unique<ImageEffectRenderer>(vulkanContext);
    std::unique_ptr<SimpleRenderer> imGuiRenderer = std::make_unique<ImGuiRenderer>(vulkanContext);

    computeRenderer->initialize(&renderTarget.resource);
    simpleMeshRenderer->initialize(&renderTarget.resource);
    imGuiRenderer->initialize(&currentSwapchainTarget);
    imageEffectRenderer->initialize(&renderTarget.resource, &currentSwapchainTarget);

    renderers.push_back(std::move(computeRenderer));
    renderers.push_back(std::move(simpleMeshRenderer));
    renderers.push_back(std::move(imGuiRenderer));
    renderers.push_back(std::move(imageEffectRenderer));
}

void RenderingEngine::exec() {
    while (!window->quitRequested()) {
        window->newFrame();

        window->handleEvents();

        processInput();

        if (window->isHidden()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw(window->getDeltaTime());

        ++frameCounter;
        commandManager.nextFrame();
    }
}

RenderingEngine::~RenderingEngine() {
    // Wait for device to finish all the commands before command buffers get deallocated!
    vkDeviceWaitIdle(vulkanContext.getDevice());

    ImageAllocator::freeImage(vulkanContext, renderTarget);
}

void RenderingEngine::draw(double deltaTime) {
    const auto device = vulkanContext.getDevice();

    // Get the render fence for the previously rendered frame
    // This sync method works okay, it allows for overlap of cmd recording
    // of a new frame while the last one is being presented,
    // however, more parallelism can be achieved if the sync primitives are more decoupled
    // with the tradeoff of allocating per-swapchain-image copies of resources (such as the g buffer)
    const auto renderFence = syncManager.getRenderFence(getFrameIndex());
    const auto swapchainImageAcquiredSemaphore = syncManager.getSwapchainImageAcquiredSemaphore(getFrameIndex());

    // Wait for previous render to finish
    vkWaitForFences(device, 1, &renderFence, VK_TRUE, UINT64_MAX);
    // Reset the render fence, we're beginning to render a new frame
    vkResetFences(device, 1, &renderFence);

    // Acquire the new swapchain image index.
    const auto swapchainImageIndex = getSwapchainImageIndex(vulkanContext.getDevice());
    // The render semaphore is synced per-swapchain-image-index
    // because the swapchain image is what's being signaled for presenting.
    // This way the frames-in-flight mechanism is completely decoupled from the swapchain!
    const auto renderSemaphore = syncManager.getRenderSemaphore(swapchainImageIndex);
    currentSwapchainTarget = vulkanContext.getSwapchainImages()[swapchainImageIndex];

    // ===============================================================================================================
    // Begin Command Recording
    // ===============================================================================================================
    commandManager.reset();
    commandManager.begin();

    for (const auto &renderer: renderers) {
        renderer->prepareFrame(deltaTime);
    }

    for (const auto &renderer: renderers) {
        renderer->execute(commandManager);
    }

    // Transition for present
    currentSwapchainTarget.transition(commandManager, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

void RenderingEngine::processInput() {
    glm::vec3 offset{0.0f};

    switch (window->getLastKeyPress()) {
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
        default:
            break;
    }

    const auto dt = static_cast<float>(window->getDeltaTime());
    camera.move(offset * dt);

    const auto mouseOffset = window->getMouseOffset();
    camera.rotate(mouseOffset.x * dt, mouseOffset.y * dt);

    const auto scrollOffset = window->getScrollOffset();
    if (scrollOffset.y != 0.0f)
        camera.changeSpeed(scrollOffset.y);

    sceneDescriptor.view = camera.getView();
    BufferAllocator::copyBufferData(vulkanContext, &sceneDescriptor, sizeof(SceneDescriptor), 0, sceneUniformBuffer);
}

void RenderingEngine::createSceneDescriptor() {
    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    sceneDescriptorLayout = layoutBuilder.build(vulkanContext.getDevice(), VK_SHADER_STAGE_VERTEX_BIT, 0);
    memoryManager.registerResource(sceneDescriptorLayout);

    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    };

    sceneDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(vulkanContext.getDevice(), resourceSizes);
    sceneDescriptorSet = sceneDescriptorAllocator->allocate(sceneDescriptorLayout);

    sceneUniformBuffer = BufferAllocator::allocateBuffer(vulkanContext, sizeof(SceneDescriptor),
                                                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                         VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memoryManager.registerResource(sceneUniformBuffer);
    sceneDescriptor = {
        .view = camera.getView(),
        .proj = camera.getProj(),
        .sunDirection = glm::normalize(glm::vec3{0.0f, -1.0f, -1.0f}),
        .sunColor = glm::vec3{1.0f, 0.9f, 0.55f}
    };
    BufferAllocator::copyBufferData(vulkanContext, &sceneDescriptor, sizeof(SceneDescriptor), 0, sceneUniformBuffer);

    DescriptorWriter::writeBuffer(vulkanContext, sceneDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  sceneUniformBuffer.buffer, sizeof(SceneDescriptor));
}

uint32_t RenderingEngine::getSwapchainImageIndex(VkDevice device) {
    uint32_t swapchainImageIndex;
    vkAcquireNextImageKHR(
        device,
        vulkanContext.getSwapchain().swapchain,
        UINT64_MAX,
        // Signal the present semaphore, so that drawing commands know that an image was acquired.
        syncManager.getSwapchainImageAcquiredSemaphore(getFrameIndex()),
        // No fences needed to signal, the CPU waits for the render commands to get issued.
        nullptr,
        &swapchainImageIndex);

    return swapchainImageIndex;
}

AllocatedImageResource RenderingEngine::createRenderTarget(const VulkanContext &vulkanContext) {
    VkImageUsageFlags usageFlags{};
    // blitting from image to swapchain
    usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; // todo check if i really need this
    // for Compute shader writes
    usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
    usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // for post-processing input
    usageFlags |= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    auto swapchainExtent = vulkanContext.getSwapchain().getExtent();

    return ImageAllocator::allocateImage2D(vulkanContext,
                                           VK_FORMAT_R16G16B16A16_SFLOAT,
                                           usageFlags,
                                           {swapchainExtent.width, swapchainExtent.height, 1},
                                           VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), // gpu-only
                                           VMA_MEMORY_USAGE_GPU_ONLY);
}

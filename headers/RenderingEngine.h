#pragma once

#include <memory>

#include "IWindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"
#include "ImGuiRenderer.h"
#include "DescriptorSetAllocator.h"
#include "ImageAllocator.h"
#include "ScopedVkMemoryManager.h"
#include "SceneDescriptor.h"
#include "Camera.h"

class RenderingEngine {
public:
    RenderingEngine();

    void initialize();

    void exec();

    ~RenderingEngine();

private:
    void processInput();

    void draw(double deltaTime);

    void createSceneDescriptor();

    // basic drawing context
    std::unique_ptr<IWindowBridge> window;
    VulkanContext vulkanContext;
    CommandManager commandManager;
    PresentSyncManager syncManager;
    AllocatedImageResource renderTarget;
    ImageResource currentSwapchainTarget;
    ScopedVkMemoryManager memoryManager;

    // Camera
    Camera camera;

    // Scene global descriptors
    // TODO: move these into the renderer, where it's actually needed...
    std::unique_ptr<DescriptorSetAllocator> sceneDescriptorAllocator;
    VkDescriptorSetLayout sceneDescriptorLayout;
    VkDescriptorSet sceneDescriptorSet;
    AllocatedBuffer sceneUniformBuffer;
    SceneDescriptor sceneDescriptor;

    // Renderers
    std::unique_ptr<SimpleRenderer> computeRenderer;
    std::unique_ptr<SimpleRenderer> simpleMeshRenderer;
    std::unique_ptr<Renderer> pbrMeshRenderer;
    std::unique_ptr<SimpleRenderer> imGuiRenderer;

    uint32_t frameCounter{0};
    uint32_t lastSwapchainImageIndex{0};

    uint32_t getSwapchainImageIndex(VkDevice device);

    static AllocatedImageResource createRenderTarget(const VulkanContext &vulkanContext);
};

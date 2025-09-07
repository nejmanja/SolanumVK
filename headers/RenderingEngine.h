#pragma once

#include <memory>

#include "IWindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"
#include "ComputeRenderer.h"
#include "SimpleMeshRenderer.h"
#include "ImGuiRenderer.h"
#include "DescriptorLayoutBuilder.h"
#include "DescriptorSetAllocator.h"
#include "ImageAllocator.h"
#include "ScopedVkMemoryManager.h"
#include "SceneDescriptor.h"
#include "Camera.h"

class RenderingEngine
{
public:
	RenderingEngine();
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
	ScopedVkMemoryManager memoryManager;

	// Camera
	Camera camera;

	// Scene global descriptors
	std::unique_ptr<DescriptorSetAllocator> sceneDescriptorAllocator;
	VkDescriptorSetLayout sceneDescriptorLayout;
	VkDescriptorSet sceneDescriptorSet;
	AllocatedBuffer sceneUniformBuffer;
	SceneDescriptor sceneDescriptor;

	// Renderers
	std::unique_ptr<IRenderer> renderer;
	std::unique_ptr<IRenderer> simpleMeshRenderer;
	std::unique_ptr<ImGuiRenderer> imGuiRenderer;

	uint32_t frameCounter{0};
	uint32_t getFrameIndex() { return frameCounter % vulkanContext.getSwapchain().framesInFlight; }
	uint32_t getSwapchainImageIndex(VkDevice device);

	static AllocatedImageResource createRenderTarget(const VulkanContext &vulkanContext);
};

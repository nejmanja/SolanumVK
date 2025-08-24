#pragma once

#include <memory>

#include "IWindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"
#include "ComputeRenderer.h"
#include "TriangleRenderer.h"
#include "ImGuiRenderer.h"
#include "DescriptorLayoutBuilder.h"
#include "DescriptorSetAllocator.h"
#include "ImageAllocator.h"

class RenderingEngine
{
public:
	RenderingEngine();
	void exec();
	~RenderingEngine();

private:
	void draw();

	// basic drawing context
	std::unique_ptr<IWindowBridge> window;
	VulkanContext vulkanContext;
	CommandManager commandManager;
	PresentSyncManager syncManager;
	AllocatedImageResource renderTarget;

	// Renderers
	std::unique_ptr<IRenderer> renderer;
	std::unique_ptr<IRenderer> triangleRenderer;
	std::unique_ptr<ImGuiRenderer> imGuiRenderer;

	uint32_t frameCounter{0};
	uint32_t getFrameIndex() { return frameCounter % vulkanContext.getSwapchain().framesInFlight; }
	uint32_t getSwapchainImageIndex(VkDevice device);

	static AllocatedImageResource CreateRenderTarget(const VulkanContext &vulkanContext);
};

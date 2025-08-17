#pragma once

#include <memory>

#include "WindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"
#include "ComputeRenderer.h"
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
	WindowBridge window;
	VulkanContext vulkanContext;
	CommandManager commandManager;
	PresentSyncManager syncManager;
	AllocatedImageResource renderTarget;

	// Renderer
	std::unique_ptr<IRenderer> renderer;

	uint32_t frameCounter{0};
	uint32_t getFrameIndex() { return frameCounter % vulkanContext.getSwapchain().framesInFlight; }

	static AllocatedImageResource CreateRenderTarget(const VulkanContext &vulkanContext);
};

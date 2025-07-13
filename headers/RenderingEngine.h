#pragma once

#include "WindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"
#include "ComputePipeline.h"

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

	// pipelines
	//ComputePipeline computePipeline;

	uint32_t frameCounter{0};
	uint32_t getFrameIndex() { return frameCounter % vulkanContext.getSwapchain().framesInFlight; }
};

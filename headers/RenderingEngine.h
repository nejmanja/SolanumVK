#pragma once

#include "WindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"

class RenderingEngine
{
public:
	RenderingEngine();
	void exec();
	~RenderingEngine();

private:
	void draw();

	WindowBridge window;
	VulkanContext vulkanContext;
	CommandManager commandManager;
	PresentSyncManager syncManager;

	uint32_t frameCounter{0};
	uint32_t getFrameIndex() { return frameCounter % vulkanContext.getSwapchain().framesInFlight; }
};

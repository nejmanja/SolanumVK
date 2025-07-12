#pragma once

#include "WindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"

class RenderingEngine
{
public:
	RenderingEngine();
	void draw();
	~RenderingEngine();
private:
	WindowBridge window;
	VulkanContext vulkanContext;
	CommandManager commandManager;
};


#pragma once

#include "WindowBridge.h"
#include "VulkanContext.h"

class RenderingEngine
{
public:
	RenderingEngine();
	void draw();
	~RenderingEngine();
private:
	WindowBridge window;
	VulkanContext vulkanContext;
};


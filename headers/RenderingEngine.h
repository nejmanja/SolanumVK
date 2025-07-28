#pragma once

#include <memory>

#include "WindowBridge.h"
#include "VulkanContext.h"
#include "CommandManager.h"
#include "PresentSyncManager.h"
#include "ComputePipeline.h"
#include "DescriptorLayoutBuilder.h"
#include "DescriptorSetAllocator.h"

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
	std::unique_ptr<ComputePipeline> computePipeline;

	// Builders
	DescriptorLayoutBuilder descriptorLayoutBuilder;
	DescriptorSetAllocator descriptorSetAllocator;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSets[2];

	uint32_t frameCounter{0};
	uint32_t getFrameIndex() { return frameCounter % vulkanContext.getSwapchain().framesInFlight; }
};

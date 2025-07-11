#include "RenderingEngine.h"

#include <thread>


RenderingEngine::RenderingEngine()
	: window(false), 
	vulkanContext(window), 
	commandManager(vulkanContext.getDevice(), vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics), 2)
{
}

void RenderingEngine::draw()
{
	while (!window.quitRequested())
	{
		window.handleEvents();

		if (window.isHidden())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}


	}
}

RenderingEngine::~RenderingEngine()
{
}

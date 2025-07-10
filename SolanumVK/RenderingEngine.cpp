#include "RenderingEngine.h"

#include <thread>


RenderingEngine::RenderingEngine()
	: window(false)
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

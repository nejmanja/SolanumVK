#pragma once

#include <vulkan/vulkan.h>

class WindowBridge
{
public:
	WindowBridge(bool resizeable);
	~WindowBridge();

	VkSurfaceKHR createSurface(VkInstance instance);

	void handleEvents();
	bool quitRequested() { return wantsQuit; }
	bool isHidden() { return minimized; }
private:

	struct SDL_Window* window{ nullptr };
	bool wantsQuit = false;
	bool minimized = false;
};


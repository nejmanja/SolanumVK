#include "WindowBridge.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "backends/imgui_impl_sdl2.h"

#include "SolanumConstants.h"

WindowBridge::WindowBridge(bool resizeable) : windowExtent(SolVK::windowWidth, SolVK::windowHeight)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WindowFlags::SDL_WINDOW_VULKAN | (resizeable ? SDL_WindowFlags::SDL_WINDOW_RESIZABLE : 0));

	window = SDL_CreateWindow(
		SolVK::AppName.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		windowExtent.width,
		windowExtent.height,
		windowFlags);

	// Also yoinked from example, main function
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	// there's also gamepad input lol
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();
	auto mainScale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
	style.ScaleAllSizes(mainScale);
	style.FontScaleDpi = mainScale;

	ImGui_ImplSDL2_InitForVulkan(window);
}

WindowBridge::~WindowBridge()
{
	SDL_DestroyWindow(window);
}

VkSurfaceKHR WindowBridge::createSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(window, instance, &surface);

	return surface;
}

void WindowBridge::handleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		// close the window when user alt-f4s or clicks the X button
		if (e.type == SDL_QUIT)
			wantsQuit = true;

		if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
			{
				minimized = true;
			}
			if (e.window.event == SDL_WINDOWEVENT_RESTORED)
			{
				minimized = false;
			}
		}

		ImGui_ImplSDL2_ProcessEvent(&e);
	}
}

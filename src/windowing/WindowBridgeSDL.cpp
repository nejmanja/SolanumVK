#include "WindowBridgeSDL.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "SolanumConstants.h"

WindowBridgeSDL::WindowBridgeSDL(bool resizeable) : windowExtent(SolVK::windowWidth, SolVK::windowHeight) {
    SDL_Init(SDL_INIT_VIDEO);

    auto windowFlags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_VULKAN
        | (resizeable ? SDL_WINDOW_RESIZABLE : 0));

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

WindowBridgeSDL::~WindowBridgeSDL() {
    SDL_DestroyWindow(window);
}

VkSurfaceKHR WindowBridgeSDL::createSurface(VkInstance instance) {
    VkSurfaceKHR surface;
    SDL_bool result = SDL_Vulkan_CreateSurface(window, instance, &surface);

    if (result != SDL_TRUE) {
        std::cout << "Failed to create surface for SDL! Aborting." << std::endl;
        abort();
    }

    return surface;
}

void WindowBridgeSDL::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        // close the window when user alt-f4s or clicks the X button
        if (e.type == SDL_QUIT)
            wantsQuit = true;

        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                minimized = true;
            }
            if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                minimized = false;
            }
        }

        processKeyPress(e);

        ImGui_ImplSDL2_ProcessEvent(&e);
    }
}

void WindowBridgeSDL::newFrame() {
    lastFrameTime = currentTime;
    currentTime = static_cast<double>(SDL_GetTicks64()) / 1000.0;
    ImGui_ImplSDL2_NewFrame();
}

void WindowBridgeSDL::processKeyPress(SDL_Event &e) {
    // TODO: add keyboard support
    lastKeyPress = KeyCode::None;
}

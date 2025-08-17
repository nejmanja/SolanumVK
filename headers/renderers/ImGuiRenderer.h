#pragma once

#include "IRenderer.h"

#include "VulkanUtils.h"
#include "VulkanContext.h"

#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

class ImGuiRenderer : public IRenderer
{
public:
    ImGuiRenderer(const VulkanContext &vulkanContext);
    ~ImGuiRenderer();

    void prepareNewFrame();
    void setup(ImageResource finalTarget) override;
    void execute(VkCommandBuffer cmd) override;

private:
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDevice device;

    VkDescriptorPool imguiPool;
};
#pragma once

#include "Renderer.h"

#include "VulkanContext.h"

class ImGuiRenderer : public Renderer {
public:
    explicit ImGuiRenderer(const VulkanContext &vulkanContext);

    ~ImGuiRenderer() override;

    void setup(ImageResource *finalTarget, double deltaTime) override;

    void execute(CommandManager &cmd) override;

private:
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;

    // This info remains the same between frames
    // Only the actual render target might change
    VkRenderingInfo renderingInfo;
    VkRenderingAttachmentInfo colorAttachment;

    VkDescriptorPool imguiPool;
};

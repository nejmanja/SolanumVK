#pragma once

#include "IRenderer.h"

#include "VulkanContext.h"

class ImGuiRenderer : public IRenderer {
public:
    explicit ImGuiRenderer(const VulkanContext &vulkanContext);

    ~ImGuiRenderer() override;

    void setup(ImageResource *finalTarget, double deltaTime) override;

    void execute(VkCommandBuffer cmd) override;

private:
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;

    // This info remains the same between frames
    // Only the actual render target might change
    VkRenderingInfo renderingInfo;
    VkRenderingAttachmentInfo colorAttachment;

    VkDescriptorPool imguiPool;
};

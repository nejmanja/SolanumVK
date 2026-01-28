#pragma once

#include "SimpleRenderer.h"

#include "VulkanContext.h"

class ImGuiRenderer : public SimpleRenderer {
public:
    explicit ImGuiRenderer(const VulkanContext &vulkanContext);

    ~ImGuiRenderer() override;

    void execute(CommandManager &cmd) override;

private:
    void setup(double deltaTime) override;

    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorSet descriptorSet{};

    // This info remains the same between frames
    // Only the actual render target might change
    VkRenderingInfo renderingInfo{};
    VkRenderingAttachmentInfo colorAttachment{};

    VkDescriptorPool imguiPool{};
};

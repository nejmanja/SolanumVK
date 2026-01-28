#pragma once

#include "Renderer.h"

#include "VulkanContext.h"

class ImGuiRenderer : public Renderer {
public:
    explicit ImGuiRenderer(const VulkanContext &vulkanContext);

    ~ImGuiRenderer() override;

    void execute(CommandManager &cmd) override;

private:
    static constexpr uint32_t TARGET_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getOutput() const { return getOutputImage(TARGET_IMAGE_INDEX); }

    void setup(double deltaTime) override;

    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorSet descriptorSet{};

    // This info remains the same between frames
    // Only the actual render target might change
    VkRenderingInfo renderingInfo{};
    VkRenderingAttachmentInfo colorAttachment{};

    VkDescriptorPool imguiPool{};
};

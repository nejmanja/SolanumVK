#pragma once
#include "GraphicsPipelineBuilder.h"
#include "Renderer.h"
#include "ScopedVkMemoryManager.h"

class ImageEffectRenderer : public Renderer {
public:
    explicit ImageEffectRenderer(const VulkanContext &vulkanContext) : Renderer(
                                                                           vulkanContext, {
                                                                               {
                                                                                   "The main input to which effects are applied."
                                                                               }
                                                                           },
                                                                           {{"The output with applied effects."}}),
                                                                       memoryManager(vulkanContext) {
    }

    void initialize(ImageResource *input, ImageResource *output) {
        Renderer::initialize({input}, {output});
    }

    void prepareFrame(double deltaTime) override {
    }

    ~ImageEffectRenderer() override {
        // TODO: add method for registering samplers in ScopedMemoryManager
        vkDestroySampler(vulkanContext.getDevice(), inputImageSampler, nullptr);
    }

protected:
    void createInputSampler();

    void initialize() override;

    void setupResources(const CommandManager &cmd) override;

    void draw(const CommandManager &cmd) override;

private:
    void createPipeline();

    void createDescriptors();

    static constexpr uint32_t SOURCE_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getInputImage() const { return Renderer::getInputImage(SOURCE_IMAGE_INDEX); }
    static constexpr uint32_t TARGET_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getOutputImage() const { return Renderer::getOutputImage(TARGET_IMAGE_INDEX); }

    VkSampler inputImageSampler{};

    ScopedVkMemoryManager memoryManager;
    std::unique_ptr<GraphicsPipeline> pipeline;
    VkRenderingAttachmentInfo colorAttachmentInfo{};
    VkRenderingInfo renderingInfo{};
    VkDescriptorSetLayout descriptorLayout{};
    VkDescriptorSet descriptorSet{};
    const VkImageLayout inputFormat = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    using Renderer::initialize;
};

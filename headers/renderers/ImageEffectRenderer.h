#pragma once
#include "Renderer.h"

class ImageEffectRenderer : Renderer {
public:
    explicit ImageEffectRenderer(const VulkanContext &vulkanContext) : Renderer(
        vulkanContext, {{"The main input to which effects are applied."}},
        {{"The output with applied effects."}}) {
    }

    void initialize(ImageResource *input, ImageResource *output) {
        Renderer::initialize({input}, {output});
    }

    void prepareFrame(double deltaTime) override {
    }

protected:
    void initialize() override;

    void setupResources(const CommandManager &cmd) override {
    }

    void draw(const CommandManager &cmd) override;

private:
    static constexpr uint32_t SOURCE_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getInputImage() const { return Renderer::getInputImage(SOURCE_IMAGE_INDEX); }
    static constexpr uint32_t TARGET_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getOutputImage() const { return Renderer::getOutputImage(TARGET_IMAGE_INDEX); }

    using Renderer::initialize;
};

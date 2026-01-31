#pragma once
#include "Renderer.h"

class SimpleRenderer : public Renderer {
public:
    explicit SimpleRenderer(const VulkanContext &vulkanContext) : Renderer(
        vulkanContext, 0, {}, 1, {{"The main color buffer to output rendered content to."}}) {
    }

    ~SimpleRenderer() override = default;

    void initialize(ImageResource *outputImage) {
        Renderer::initialize({}, {outputImage});
    }

    void prepareFrame(double deltaTime) override = 0;

protected:
    void initialize() override = 0;

    void draw(const CommandManager &cmd) override = 0;

    static constexpr uint32_t TARGET_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getOutput() const { return getOutputImage(TARGET_IMAGE_INDEX); }

private:
    using Renderer::initialize;
};

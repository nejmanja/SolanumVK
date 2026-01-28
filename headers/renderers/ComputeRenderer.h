#pragma once

#include <fastgltf/types.hpp>

#include "Camera.h"
#include "Renderer.h"
#include "ComputePipeline.h"
#include "VulkanContext.h"

class ComputeRenderer : public Renderer {
public:
    ComputeRenderer(const VulkanContext &vulkanContext, const Camera *camera);

    ~ComputeRenderer() override;

    void execute(CommandManager &cmd) override;

private:
    bool firstRun = true;

    static constexpr uint32_t TARGET_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getOutput() const { return getOutputImage(TARGET_IMAGE_INDEX); }

    void setup(double deltaTime) override;

    // Pipeline with which rendering will happen
    std::unique_ptr<ComputePipeline> pipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDevice device;

    const Camera *camera;
};

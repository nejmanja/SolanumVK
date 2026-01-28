#pragma once

#include <fastgltf/types.hpp>

#include "Camera.h"
#include "Renderer.h"
#include "ComputePipeline.h"
#include "VulkanContext.h"

class ComputeRenderer : public Renderer {
public:
    ComputeRenderer(const VulkanContext &vulkanContext, const ImageResource &renderTarget, const Camera *camera);

    ~ComputeRenderer() override;

    void setup(ImageResource *finalTarget, double deltaTime) override;

    void execute(CommandManager &cmd) override;

private:
    // Pipeline with which rendering will happen
    std::unique_ptr<ComputePipeline> pipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDevice device;

    const Camera *camera;
};

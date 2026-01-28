#pragma once

#include <fastgltf/types.hpp>

#include "Camera.h"
#include "ComputePipeline.h"
#include "SimpleRenderer.h"
#include "VulkanContext.h"

class ComputeRenderer : public SimpleRenderer {
public:
    ComputeRenderer(const VulkanContext &vulkanContext, const Camera *camera);

    ~ComputeRenderer() override;

    void execute(CommandManager &cmd) override;

private:
    bool firstRun = true;

    void setup(double deltaTime) override;

    // Pipeline with which rendering will happen
    std::unique_ptr<ComputePipeline> pipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDevice device;

    const Camera *camera;
};

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

    void prepareFrame(double deltaTime) override {
    }

protected:
    void initialize() override;

    void setupResources(const CommandManager &cmd) override;

    void draw(const CommandManager &cmd) override;

private:
    // Pipeline with which rendering will happen
    std::unique_ptr<ComputePipeline> pipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDevice device;

    const Camera *camera;
};

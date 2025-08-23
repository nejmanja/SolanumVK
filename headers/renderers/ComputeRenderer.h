#pragma once

#include "IRenderer.h"
#include "ComputePipeline.h"
#include "VulkanContext.h"

class ComputeRenderer : public IRenderer
{
public:
    ComputeRenderer(const VulkanContext &vulkanContext, const ImageResource &renderTarget);
    ~ComputeRenderer();

    void setup(ImageResource finalTarget) override;
    void execute(VkCommandBuffer cmd) override;

private:
    // Pipeline with which rendering will happen
    std::unique_ptr<ComputePipeline> pipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDevice device;
};
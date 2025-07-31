#pragma once

#include "IRenderer.h"
#include "ComputePipeline.h"
#include "VulkanContext.h"

class ComputeRenderer : public IRenderer
{
public:
    ComputeRenderer(const VulkanContext &vulkanContext);
    ~ComputeRenderer();

    void setup(ImageResource finalTarget) override;
    void exec(VkCommandBuffer cmd) override;

private:
    VkDescriptorSet descriptorSet;
    VkDevice device;
};
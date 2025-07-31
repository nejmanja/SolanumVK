#pragma once

#include "IRenderer.h"
#include "ComputePipeline.h"
#include "VulkanContext.h"

class ComputeRenderer : public IRenderer
{
public:
    ComputeRenderer(const VulkanContext &vulkanContext);
    ~ComputeRenderer();

    void setup(SwapchainImageResource finalTarget) override;
    void exec(VkCommandBuffer cmd) override;

private:
    std::vector<VkDescriptorSet> descriptorSets;
    VkDevice device;
};
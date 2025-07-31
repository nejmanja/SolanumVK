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
    void execute(VkCommandBuffer cmd) override;

private:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDevice device;
};
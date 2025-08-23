#pragma once

#include "IPipeline.h"

class GraphicsPipeline : public IPipeline
{
public:
    GraphicsPipeline(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);
    ~GraphicsPipeline();

    void bindPushConstants(void *pushConstantData) override;
    void bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets) override;

    // Dynamic state setup
    void setViewport(VkViewport *viewport);
    void setScissor(VkRect2D *scissor);

    void bind(VkCommandBuffer cmd) override;

    void execute() override;

private:
};
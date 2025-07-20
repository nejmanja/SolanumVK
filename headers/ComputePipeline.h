#pragma once

#include "IPipeline.h"

class ComputePipeline : public IPipeline
{
public:
    ComputePipeline(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);
    ~ComputePipeline() override;
    void execute();
    void bindPushConstants(void *pushConstantData) override;
    void bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets) override;

private:
    void bind(VkCommandBuffer cmd) override;
};
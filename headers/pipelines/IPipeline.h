#pragma once

#include <vulkan/vulkan.h>

class IPipeline
{
public:
    IPipeline(VkDevice device) { this->device = device; }
    IPipeline(VkDevice device, VkPipelineLayout layout, VkPipeline pipeline)
        : device(device), layout(layout), pipeline(pipeline) {}

    virtual void bindPushConstants(void *pushConstantData) = 0;
    virtual void bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets) = 0;

    // Binds the pipeline to a command buffer.
    virtual void bind(VkCommandBuffer cmd) = 0;

    virtual void execute() = 0;

    virtual ~IPipeline() {};

protected:
    // Binds the pipeline to a command buffer.
    void bind(VkCommandBuffer cmd, VkPipelineBindPoint bindPoint)
    {
        boundCommandBuffer = cmd;
        vkCmdBindPipeline(boundCommandBuffer, bindPoint, pipeline);
    }

    void bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets, VkPipelineBindPoint bindPoint)
    {
        vkCmdBindDescriptorSets(boundCommandBuffer, bindPoint, layout, 0, setCount, sets, 0, nullptr);
    }

    void bindPushConstants(void *pushConstantData, VkShaderStageFlags stageFlags)
    {
        vkCmdPushConstants(boundCommandBuffer, layout, stageFlags, 0, sizeof(pushConstantData), pushConstantData);
    }

    // The command buffer to which the pipeline has been bound to.
    VkCommandBuffer boundCommandBuffer;
    // The layout for the pipeline, since it has to be destroyed separately
    VkPipelineLayout layout;
    // The actual pipeline handle
    VkPipeline pipeline;
    // Device for which the pipeline gets created
    VkDevice device;
};
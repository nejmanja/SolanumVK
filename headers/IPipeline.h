#pragma once

#include <vulkan/vulkan.h>

class IPipeline
{
public:
    IPipeline(VkDevice device) { this->device = device; }
    virtual void execute(VkCommandBuffer cmd) = 0;
    virtual ~IPipeline() {};

protected:
    virtual void bind(VkCommandBuffer cmd) = 0;
    // The layout for the pipeline, since it has to be destroyed separately
    VkPipelineLayout layout;
    // The actual pipeline handle
    VkPipeline pipeline;
    // Device for which the pipeline gets created
    VkDevice device;
};
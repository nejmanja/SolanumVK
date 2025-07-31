#pragma once

#include <memory>

#include "IPipeline.h"
#include "DescriptorSetAllocator.h"
#include "ImageResources.h"

class IRenderer
{
public:
    IRenderer() {}
    virtual ~IRenderer() {}

    // Used before recording commands. May include setup for descriptor sets and push constants.
    // Sets the final render target to which the renderer should write.
    virtual void setup(SwapchainImageResource finalTarget) { this->finalTarget = finalTarget; }
    // Records and submits commands to the GPU using the pipeline.
    virtual void exec(VkCommandBuffer cmd) = 0;

protected:
    SwapchainImageResource finalTarget;

    // Pipeline with which rendering will happen
    std::unique_ptr<IPipeline> pipeline;
    // For pipeline-specific descriptors
    std::unique_ptr<DescriptorSetAllocator> rendererDescriptorAllocator;
};
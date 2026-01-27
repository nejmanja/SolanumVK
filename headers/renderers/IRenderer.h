#pragma once

#include <memory>

#include "VulkanContext.h"
#include "DescriptorSetAllocator.h"
#include "ImageResources.h"

class IRenderer {
public:
    explicit IRenderer(const VulkanContext &vulkanContext) : vulkanContext{vulkanContext} {
    }

    virtual ~IRenderer() = default;

    // Used before recording commands. May include setup for descriptor sets and push constants.
    // Sets the final render target to which the renderer should write.
    virtual void setup(ImageResource *finalTarget, double deltaTime) { this->finalTarget = finalTarget; }

    // Records and submits commands to the GPU using the pipeline.
    virtual void execute(VkCommandBuffer cmd) = 0;

protected:
    const VulkanContext &vulkanContext;
    ImageResource *finalTarget{};

    // For pipeline-specific descriptors
    std::unique_ptr<DescriptorSetAllocator> rendererDescriptorAllocator;
};

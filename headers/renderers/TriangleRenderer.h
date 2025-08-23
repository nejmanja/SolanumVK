#pragma once

#include "IRenderer.h"
#include "VulkanContext.h"
#include "GraphicsPipeline.h"

class TriangleRenderer : public IRenderer
{
public:
    TriangleRenderer(const VulkanContext &vulkanContext);
    ~TriangleRenderer();

    void setup(ImageResource finalTarget) override;
    void execute(VkCommandBuffer cmd) override;

private:
    // Pipeline with which rendering will happen
    std::unique_ptr<GraphicsPipeline> pipeline;
};
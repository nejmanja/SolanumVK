#include "TriangleRenderer.h"

TriangleRenderer::TriangleRenderer(const VulkanContext &vulkanContext)
{
    // TODO: actual descriptor layout
    pipeline = std::make_unique<GraphicsPipeline>(vulkanContext.getDevice(), VkDescriptorSetLayout{});
}

TriangleRenderer::~TriangleRenderer()
{
}

void TriangleRenderer::setup(ImageResource finalTarget)
{
}

void TriangleRenderer::execute(VkCommandBuffer cmd)
{
}

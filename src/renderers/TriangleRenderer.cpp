#include "TriangleRenderer.h"

#include "SolanumConstants.h"

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
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)SolVK::windowWidth;
    viewport.height = (float)SolVK::windowHeight;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = SolVK::windowWidth;
    scissor.extent.height = SolVK::windowHeight;

    pipeline->setViewport(&viewport);
    pipeline->setScissor(&scissor);
}

void TriangleRenderer::execute(VkCommandBuffer cmd)
{
    VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO, .pNext = nullptr};

    vkCmdBeginRendering(cmd, &renderingInfo);
    pipeline->bind(cmd);

    // do the actual rendering...

    vkCmdEndRendering(cmd);
}

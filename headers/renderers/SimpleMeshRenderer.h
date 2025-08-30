#pragma once

#include "IRenderer.h"
#include "VulkanContext.h"
#include "GraphicsPipeline.h"
#include "SimpleMeshData.h"

class SimpleMeshRenderer : public IRenderer
{
public:
    SimpleMeshRenderer(const VulkanContext &vulkanContext);
    ~SimpleMeshRenderer();

    void setup(ImageResource finalTarget) override;
    void execute(VkCommandBuffer cmd) override;

private:
    void buildPipeline(const VulkanContext &vulkanContext);
    void createMeshBuffers(const VulkanContext &vulkanContext);

    VkViewport viewport;
    VkRect2D scissor;

    VmaAllocator vmaAllocator;

    SimpleMeshData meshData;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VmaAllocation vertexBufferAllocation;
    VmaAllocation indexBufferAllocation;

    // Pipeline with which rendering will happen
    std::unique_ptr<GraphicsPipeline> pipeline;
};
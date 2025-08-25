#include "TriangleRenderer.h"

#include "SolanumConstants.h"
#include "VulkanUtils.h"

TriangleRenderer::TriangleRenderer(const VulkanContext &vulkanContext)
    : viewport{
          .x = 0,
          .y = 0,
          .width = (float)SolVK::windowWidth,
          .height = (float)SolVK::windowHeight,
          .minDepth = 0.0f,
          .maxDepth = 1.0f},
      scissor{.offset{0, 0}, .extent{SolVK::windowWidth, SolVK::windowHeight}}
{
    // TODO: actual descriptor layout
    pipeline = std::make_unique<GraphicsPipeline>(vulkanContext.getDevice(), nullptr);

    // TODO: move this into a buffer allocator class
    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = 3 * sizeof(GraphicsPipeline::Vertex),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE};
    VmaAllocationCreateInfo vmaInfo{
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU};

    vmaAllocator = vulkanContext.getVmaAllocator();

    auto result = vmaCreateBuffer(vmaAllocator, &bufferCreateInfo, &vmaInfo, &vertexBuffer, &vertexBufferAllocation, nullptr);
    VulkanUtils::CheckVkResult(result);

    // Y-axis goes top (-1) to bottom (+1)!
    std::vector<GraphicsPipeline::Vertex> vertices{};
    vertices.push_back({.pos = glm::vec2{-0.5f, 0.5f},
                        .color = glm::vec3{1, 0, 0}});
    vertices.push_back({.pos = glm::vec2{0.5f, 0.5f},
                        .color = glm::vec3{0, 1, 0}});
    vertices.push_back({.pos = glm::vec2{0.0f, -0.5f},
                        .color = glm::vec3{0, 0, 1}});

    // Map the memory and copy the vertex data
    void *data;
    result = vmaMapMemory(vmaAllocator, vertexBufferAllocation, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(vertices.size() * sizeof(GraphicsPipeline::Vertex)));
    vmaUnmapMemory(vmaAllocator, vertexBufferAllocation);
}

TriangleRenderer::~TriangleRenderer()
{
    vmaDestroyBuffer(vmaAllocator, vertexBuffer, vertexBufferAllocation);
}

void TriangleRenderer::setup(ImageResource finalTarget)
{
    IRenderer::setup(finalTarget);

    viewport.width = (float)finalTarget.imageExtent.width;
    viewport.height = (float)finalTarget.imageExtent.height;

    scissor.extent.width = finalTarget.imageExtent.width;
    scissor.extent.height = finalTarget.imageExtent.height;
}

void TriangleRenderer::execute(VkCommandBuffer cmd)
{
    VkRenderingAttachmentInfo colorAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = finalTarget.imageView,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        // no MSAA
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // compute pass will draw before this :)
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {}};

    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = VkRect2D{.offset = {0, 0}, .extent = {finalTarget.imageExtent.width, finalTarget.imageExtent.height}},
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = VK_NULL_HANDLE,
        .pStencilAttachment = VK_NULL_HANDLE};

    vkCmdBeginRendering(cmd, &renderingInfo);
    pipeline->bind(cmd);

    pipeline->setViewport(&viewport);
    pipeline->setScissor(&scissor);

    VkDeviceSize offset{0};
    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRendering(cmd);
}

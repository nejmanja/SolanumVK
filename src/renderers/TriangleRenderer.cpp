#include "TriangleRenderer.h"

#include "SolanumConstants.h"
#include "VulkanUtils.h"

#include "GraphicsPipelineBuilder.h"

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
    buildPipeline(vulkanContext);

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

void TriangleRenderer::buildPipeline(const VulkanContext &vulkanContext)
{
    GraphicsPipelineBuilder builder{vulkanContext};

    // TODO create a Vertex descriptor that can spit out attribute descriptions dynamically...
    builder.addVertexBinding(0, sizeof(GraphicsPipeline::Vertex),
                             std::vector<VkVertexInputAttributeDescription>{
                                 // Position
                                 VkVertexInputAttributeDescription{
                                     .location = 0,
                                     .format = VK_FORMAT_R32G32_SFLOAT,
                                     .offset = 0},
                                 // Color
                                 VkVertexInputAttributeDescription{
                                     .location = 1,
                                     .format = VK_FORMAT_R32G32B32_SFLOAT,
                                     .offset = 2 * sizeof(float)},
                             });

    builder.addColorAttachmentFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
    builder.addShaderModule("../../shaders/triangle.vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT);
    builder.addShaderModule("../../shaders/triangle.frag.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT);
    builder.setCullMode(VK_CULL_MODE_BACK_BIT);
    builder.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    builder.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    builder.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

    pipeline = builder.build();

    builder.reset();
}

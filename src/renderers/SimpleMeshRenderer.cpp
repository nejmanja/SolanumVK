#include "SimpleMeshRenderer.h"

#include "SolanumConstants.h"
#include "VulkanUtils.h"

#include "GraphicsPipelineBuilder.h"

#include "MeshLoader.h"

SimpleMeshRenderer::SimpleMeshRenderer(const VulkanContext &vulkanContext)
    : viewport{
          .x = 0,
          .y = 0,
          .width = (float)SolVK::windowWidth,
          .height = (float)SolVK::windowHeight,
          .minDepth = 0.0f,
          .maxDepth = 1.0f},
      scissor{.offset{0, 0}, .extent{SolVK::windowWidth, SolVK::windowHeight}}
{
    meshData = MeshLoader::loadSimpleMesh("../../assets/vertexColorCube.glb");

    buildPipeline(vulkanContext);
    createMeshBuffers(vulkanContext);
}

SimpleMeshRenderer::~SimpleMeshRenderer()
{
    vmaDestroyBuffer(vmaAllocator, vertexBuffer, vertexBufferAllocation);
    vmaDestroyBuffer(vmaAllocator, indexBuffer, indexBufferAllocation);
}

void SimpleMeshRenderer::setup(ImageResource finalTarget)
{
    IRenderer::setup(finalTarget);

    viewport.width = (float)finalTarget.imageExtent.width;
    viewport.height = (float)finalTarget.imageExtent.height;

    scissor.extent.width = finalTarget.imageExtent.width;
    scissor.extent.height = finalTarget.imageExtent.height;
}

void SimpleMeshRenderer::execute(VkCommandBuffer cmd)
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
    vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(cmd, meshData.getIndices().size(), 1, 0, 0, 0);

    vkCmdEndRendering(cmd);
}

void SimpleMeshRenderer::buildPipeline(const VulkanContext &vulkanContext)
{
    GraphicsPipelineBuilder builder{vulkanContext};
    builder.addVertexBinding(meshData.getFormatDescriptor().getBindingDescriptors()[0]);

    builder.addColorAttachmentFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
    builder.addShaderModule("../../shaders/triangle.vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT);
    builder.addShaderModule("../../shaders/triangle.frag.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT);
    builder.setCullMode(VK_CULL_MODE_BACK_BIT);
    builder.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    builder.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    builder.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

    pipeline = builder.build();
}

void SimpleMeshRenderer::createMeshBuffers(const VulkanContext &vulkanContext)
{
    auto vertexBufferSize = meshData.getVertexSize() * meshData.getVertexCount();

    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = vertexBufferSize,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE};
    VmaAllocationCreateInfo vmaInfo{
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU};

    vmaAllocator = vulkanContext.getVmaAllocator();

    auto result = vmaCreateBuffer(vmaAllocator, &bufferCreateInfo, &vmaInfo, &vertexBuffer, &vertexBufferAllocation, nullptr);
    VulkanUtils::CheckVkResult(result);

    // Map the memory and copy the vertex data
    void *data;
    result = vmaMapMemory(vmaAllocator, vertexBufferAllocation, &data);
    memcpy(data, meshData.getRawVertexData(), static_cast<size_t>(vertexBufferSize));
    vmaUnmapMemory(vmaAllocator, vertexBufferAllocation);

    bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferCreateInfo.size = meshData.getIndices().size() * sizeof(uint32_t);
    result = vmaCreateBuffer(vmaAllocator, &bufferCreateInfo, &vmaInfo, &indexBuffer, &indexBufferAllocation, nullptr);
    VulkanUtils::CheckVkResult(result);

    void *indexData;
    result = vmaMapMemory(vmaAllocator, indexBufferAllocation, &indexData);
    memcpy(indexData, meshData.getIndices().data(), static_cast<size_t>(meshData.getIndices().size() * sizeof(uint32_t)));
    vmaUnmapMemory(vmaAllocator, indexBufferAllocation);
}

#include "SimpleMeshRenderer.h"

#include "SolanumConstants.h"
#include "VulkanUtils.h"

#include "GraphicsPipelineBuilder.h"
#include "DescriptorLayoutBuilder.h"

#include "MeshLoader.h"

SimpleMeshRenderer::SimpleMeshRenderer(const VulkanContext &vulkanContext)
    : viewport{
          .x = 0,
          .y = 0,
          .width = (float)SolVK::windowWidth,
          .height = (float)SolVK::windowHeight,
          .minDepth = 0.0f,
          .maxDepth = 1.0f},
      scissor{.offset{0, 0}, .extent{SolVK::windowWidth, SolVK::windowHeight}}, meshUploader{vulkanContext}, memoryManager{vulkanContext}

{
    auto device = vulkanContext.getDevice();

    meshData = MeshLoader::loadSimpleMesh("../../assets/vertexColorCube.glb");
    meshUploader.uploadMesh(meshData);
    memoryManager.registerResource(meshData);

    createDescriptors(vulkanContext);

    buildPipeline(vulkanContext);
}

SimpleMeshRenderer::~SimpleMeshRenderer()
{
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
    pipeline->bindDescriptorSets(1, &transformUniformDescriptorSet);

    pipeline->setViewport(&viewport);
    pipeline->setScissor(&scissor);

    VkDeviceSize offset{0};
    auto vertexBuffer = meshData.getVertexBuffer().buffer;
    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
    vkCmdBindIndexBuffer(cmd, meshData.getIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(cmd, meshData.getIndices().size(), 1, 0, 0, 0);

    vkCmdEndRendering(cmd);
}

void SimpleMeshRenderer::createDescriptors(const VulkanContext &vulkanContext)
{
    auto device = vulkanContext.getDevice();

    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    transformUniformLayout = layoutBuilder.build(device, VK_SHADER_STAGE_VERTEX_BIT, 0);
    memoryManager.registerResource(transformUniformLayout);

    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}};
    rendererDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(device, resourceSizes);

    transformUniformDescriptorSet = rendererDescriptorAllocator->allocate(transformUniformLayout);

    BufferAllocator bufferAllocator(vulkanContext);
    transformBuffer = bufferAllocator.allocateBuffer(sizeof(Transform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    memoryManager.registerResource(transformBuffer);

    Transform transform{
        .model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.2f}),
        .view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}),
        .projection = glm::perspective(50.0f, 800.0f / 600.0f, 0.1f, 1000.0f)};
    bufferAllocator.copyBufferData(&transform, sizeof(Transform), transformBuffer);

    descriptorWriter = std::make_unique<DescriptorWriter>(vulkanContext);
    descriptorWriter->writeBuffer(transformUniformDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, transformBuffer.buffer, sizeof(Transform));
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
    builder.addDescriptorSetLayout(transformUniformLayout);

    pipeline = builder.build();
}

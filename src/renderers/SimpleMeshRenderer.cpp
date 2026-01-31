#include "SimpleMeshRenderer.h"

#include "BufferAllocator.h"
#include "SolanumConstants.h"
#include "VulkanUtils.h"

#include "ImageAllocator.h"
#include "GraphicsPipelineBuilder.h"
#include "DescriptorLayoutBuilder.h"
#include "DescriptorWriter.h"
#include "MeshLoader.h"
#include "MeshUploader.h"

SimpleMeshRenderer::SimpleMeshRenderer(const VulkanContext &vulkanContext,
                                       const VkDescriptorSetLayout sceneDescriptorLayout,
                                       const VkDescriptorSet sceneDescriptorSet)
    : SimpleRenderer(vulkanContext),
      sceneDescriptorSet{sceneDescriptorSet},
      viewport{
          .x = 0,
          .y = 0,
          .width = (float) SolVK::windowWidth,
          .height = (float) SolVK::windowHeight,
          .minDepth = 0.0f,
          .maxDepth = 1.0f
      }, scissor{.offset{0, 0}, .extent{SolVK::windowWidth, SolVK::windowHeight}},
      memoryManager{vulkanContext} {
    meshData = MeshLoader::loadSimpleMesh("../../assets/greenMonke.glb");

    createDepthTarget();
    createDescriptors();
    buildPipeline(sceneDescriptorLayout);
}

SimpleMeshRenderer::~SimpleMeshRenderer() {
    delete depthTarget;
}

void SimpleMeshRenderer::initialize() {
    viewport.width = static_cast<float>(getOutput()->getExtent().width);
    viewport.height = static_cast<float>(getOutput()->getExtent().height);

    scissor.extent.width = getOutput()->getExtent().width;
    scissor.extent.height = getOutput()->getExtent().height;

    MeshUploader::uploadMesh(vulkanContext, meshData);
    memoryManager.registerResource(meshData);
}

void SimpleMeshRenderer::draw(const CommandManager &cmd) {
    auto *output = getOutput();

    VkRenderingAttachmentInfo colorAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = output->getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        // no MSAA
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // compute pass will draw before this :)
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {}
    };

    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = VkRect2D{
            .offset = {0, 0}, .extent = {output->getExtent().width, output->getExtent().height}
        },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo,
        .pStencilAttachment = VK_NULL_HANDLE
    };

    const auto cmdBuffer = cmd.get();


    vkCmdBeginRendering(cmdBuffer, &renderingInfo);
    pipeline->bind(cmdBuffer);

    VkDescriptorSet descriptorSets[2] = {sceneDescriptorSet, transformUniformDescriptorSet};
    pipeline->bindDescriptorSets(2, descriptorSets);

    pipeline->setViewport(&viewport);
    pipeline->setScissor(&scissor);

    VkDeviceSize offset{0};
    auto vertexBuffer = meshData.getVertexBuffer().buffer;
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, &offset);
    vkCmdBindIndexBuffer(cmdBuffer, meshData.getIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(cmdBuffer, meshData.getIndices().size(), 1, 0, 0, 0);

    vkCmdEndRendering(cmdBuffer);
}

void SimpleMeshRenderer::createDepthTarget() {
    auto depth = ImageAllocator::allocateImage2D(
        vulkanContext,
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VkExtent3D{SolVK::windowWidth, SolVK::windowHeight, 1},
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);
    const auto depthResource = depth.resource;
    depthTarget = new AllocatedImageResource{
        .resource = {
            depthResource.getImage(),
            depthResource.getImageView(),
            depthResource.getExtent(),
            depthResource.getImageLayout(),
            depthResource.getFormat()
        },
        .allocation = depth.allocation
    };


    memoryManager.registerResource(*depthTarget);

    depthAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = depthTarget->resource.getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        // no MSAA
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue = VkClearValue{.depthStencil{.depth{1.0f}, .stencil{0}}}
    };
}

void SimpleMeshRenderer::createDescriptors() {
    auto device = vulkanContext.getDevice();

    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    transformUniformLayout = layoutBuilder.build(device, VK_SHADER_STAGE_VERTEX_BIT, 0);
    memoryManager.registerResource(transformUniformLayout);

    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    };
    rendererDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(device, resourceSizes);

    transformUniformDescriptorSet = rendererDescriptorAllocator->allocate(transformUniformLayout);

    transformBuffer = BufferAllocator::allocateBuffer(vulkanContext, sizeof(Transform),
                                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                                      VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memoryManager.registerResource(transformBuffer);

    transform = {
        .model = glm::scale(glm::mat4{1.0f}, glm::vec3{5.0f})
    };
    BufferAllocator::copyBufferData(vulkanContext, &transform, sizeof(Transform), transformBuffer);

    DescriptorWriter::writeBuffer(vulkanContext, transformUniformDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  transformBuffer.buffer, sizeof(Transform));
}

void SimpleMeshRenderer::buildPipeline(const VkDescriptorSetLayout sceneDescriptorLayout) {
    GraphicsPipelineBuilder builder{vulkanContext};
    builder.addVertexBinding(meshData.getFormatDescriptor().getBindingDescriptors()[0]);

    builder.addColorAttachmentFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
    builder.setDepthAttachmentFormat(VK_FORMAT_D32_SFLOAT);
    builder.enableDepthTest(true);
    builder.enableDepthWrite(true);
    builder.addShaderModule("../../shaders/simpleMesh.vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT);
    builder.addShaderModule("../../shaders/simpleMesh.frag.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT);
    builder.setCullMode(VK_CULL_MODE_BACK_BIT);
    builder.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    builder.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    builder.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
    builder.addDescriptorSetLayout(sceneDescriptorLayout);
    builder.addDescriptorSetLayout(transformUniformLayout);

    pipeline = builder.build();
}

void SimpleMeshRenderer::setupResources(const CommandManager &cmd) {
    BufferAllocator::copyBufferData(vulkanContext, &transform, sizeof(Transform), transformBuffer);
    depthTarget->resource.transition(cmd, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    getOutput()->transition(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void SimpleMeshRenderer::prepareFrame(double deltaTime) {
    transform.model = glm::rotate(transform.model, static_cast<float>(deltaTime), glm::vec3{0.0f, 1.0f, 0.0f});
}

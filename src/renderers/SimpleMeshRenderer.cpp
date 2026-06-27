#include "SimpleMeshRenderer.h"

#include "BufferAllocator.h"
#include "SolanumConstants.h"
#include "VulkanUtils.h"

#include "ImageAllocator.h"
#include "GraphicsPipelineBuilder.h"
#include "DescriptorLayoutBindings.h"
#include "DescriptorWriter.h"
#include "MeshLoader.h"
#include "MeshUploader.h"

SimpleMeshRenderer::SimpleMeshRenderer(const VulkanContext &vulkanContext,
                                       const VkDescriptorSetLayout sceneDescriptorLayout,
                                       const VkDescriptorSet sceneDescriptorSet)
    : SimpleRenderer("SimpleMeshRenderer", vulkanContext),
      sceneDescriptorSet{sceneDescriptorSet},
      viewport{
          .x = 0,
          .y = 0,
          .width = (float) SolVK::windowWidth,
          .height = (float) SolVK::windowHeight,
          .minDepth = 0.0f,
          .maxDepth = 1.0f
      }, scissor{.offset{0, 0}, .extent{SolVK::windowWidth, SolVK::windowHeight}},
      memoryManager{vulkanContext},
      meshData(MeshLoader::loadMesh(VertexAttributes::Position | VertexAttributes::Normal | VertexAttributes::Color,
                                    "../../assets/greenMonke.glb")) {
    createDepthTarget();
    createDescriptors();
    buildPipeline(sceneDescriptorLayout);
}

SimpleMeshRenderer::~SimpleMeshRenderer() {
    delete depthTarget;
}

void SimpleMeshRenderer::initialize() {
    viewport.width = static_cast<float>(getOutputImage()->getExtent().width);
    viewport.height = static_cast<float>(getOutputImage()->getExtent().height);

    scissor.extent.width = getOutputImage()->getExtent().width;
    scissor.extent.height = getOutputImage()->getExtent().height;

    auto gpuMeshData = MeshUploader::uploadMesh(vulkanContext, meshData.value());
    memoryManager.registerResource(gpuMeshData);

    monkeyNode = scene.addNode({.name = "Monkey", .transform = Transform{glm::mat4{1.0f}}, .gpuMesh = gpuMeshData});
    monkeyNode->addChild(
        {
            .name = "Monkey moon",
            .transform = Transform{glm::vec3(2.0f, 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f)},
            .gpuMesh = gpuMeshData
        });
    BufferAllocator::copyBufferData(vulkanContext, &monkeyNode->getLocalTransform(), sizeof(glm::mat4), 0,
                                    transformBuffer);
}

void SimpleMeshRenderer::draw(const CommandManager &cmd) {
    auto *output = getOutputImage();

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

    pipeline->setViewport(&viewport);
    pipeline->setScissor(&scissor);

    int i = 0;
    for (auto &node: scene.getRenderList()) {
        VkDescriptorSet descriptorSets[2] = {sceneDescriptorSet, descriptorModule->getDescriptorSet(i)};
        pipeline->bindDescriptorSets(2, descriptorSets);

        auto mesh = node->getMeshData();
        mesh->bind(cmdBuffer);
        mesh->draw(cmdBuffer);

        i = (i + 1) % 2;
    }

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

    DescriptorLayoutBindings layoutBindings{};
    layoutBindings.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    descriptorModule.emplace(layoutBindings.createModule(device, VK_SHADER_STAGE_VERTEX_BIT, 0));

    rendererDescriptorMemoryManager = std::make_unique<DescriptorMemoryManager>(device);
    rendererDescriptorMemoryManager->addBindings(descriptorModule->getBindings());
    rendererDescriptorMemoryManager->initialize();

    descriptorModule->createSet(*rendererDescriptorMemoryManager);
    descriptorModule->createSet(*rendererDescriptorMemoryManager);

    transformBuffer = BufferAllocator::allocateBuffer(vulkanContext, sizeof(glm::mat4),
                                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                                      VMA_ALLOCATION_CREATE_MAPPED_BIT);
    otherTransformBuffer = BufferAllocator::allocateBuffer(vulkanContext, sizeof(glm::mat4),
                                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                           VMA_MEMORY_USAGE_CPU_TO_GPU,
                                                           VMA_ALLOCATION_CREATE_MAPPED_BIT);


    memoryManager.registerResource(transformBuffer);
    memoryManager.registerResource(otherTransformBuffer);

    DescriptorWriter::writeBuffer(vulkanContext, descriptorModule->getDescriptorSet(0),
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  transformBuffer.buffer, sizeof(glm::mat4));

    DescriptorWriter::writeBuffer(vulkanContext, descriptorModule->getDescriptorSet(1),
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  otherTransformBuffer.buffer, sizeof(glm::mat4));
}

void SimpleMeshRenderer::buildPipeline(const VkDescriptorSetLayout sceneDescriptorLayout) {
    GraphicsPipelineBuilder builder{vulkanContext};
    builder.addVertexBinding(0, 4 * 3 * 3, meshData->getAttributeDescriptions());

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
    builder.addDescriptorSetLayout(descriptorModule->getLayout());

    pipeline = builder.build();
}

void SimpleMeshRenderer::setupResources(const CommandManager &cmd) {
    int i = 0;
    for (auto &node: scene.getRenderList()) {
        auto transformMatrix = node->getGlobalTransformMatrix();
        auto &buffer = i == 0 ? transformBuffer : otherTransformBuffer;
        BufferAllocator::copyBufferData(vulkanContext, &transformMatrix, sizeof(glm::mat4), 0,
                                        buffer);
        i++;
    }
    depthTarget->resource.transition(cmd, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    getOutputImage()->transition(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void SimpleMeshRenderer::prepareFrame(double deltaTime) {
    for (auto &node: scene.getRenderList()) {
        node->setLocalTransformMatrix(glm::rotate(node->getLocalTransformMatrix(),
                                                  static_cast<float>(deltaTime), glm::vec3{0.0f, 1.0f, 0.0f}));
    }
}

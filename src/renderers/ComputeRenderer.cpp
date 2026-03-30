#include "ComputeRenderer.h"

#include <glm/vec3.hpp>

#include "DescriptorLayoutBindings.h"
#include "DescriptorWriter.h"

ComputeRenderer::ComputeRenderer(const VulkanContext &vulkanContext,
                                 const Camera *camera)
    : SimpleRenderer(vulkanContext), camera(camera) {
    const auto device = vulkanContext.getDevice();

    DescriptorLayoutBindings layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    descriptorModule = layoutBuilder.createModule(device, VK_SHADER_STAGE_COMPUTE_BIT, 0);

    // TODO: check if you can replace raw layout getter with whole module
    pipeline = std::make_unique<ComputePipeline>(device, descriptorModule->getLayout());
    rendererDescriptorMemoryManager = std::make_unique<DescriptorMemoryManager>(device);
    rendererDescriptorMemoryManager->addBindings(descriptorModule->getBindings());
    rendererDescriptorMemoryManager->initialize();

    descriptorModule->createSet(*rendererDescriptorMemoryManager);
}

ComputeRenderer::~ComputeRenderer() {
    rendererDescriptorMemoryManager->resetPools();
}

void ComputeRenderer::initialize() {
    DescriptorWriter::writeImage(vulkanContext, descriptorModule->getDescriptorSet(0), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                 getOutputImage()->getImageView(),
                                 VK_IMAGE_LAYOUT_GENERAL);
}

void ComputeRenderer::setupResources(const CommandManager &cmd) {
    getOutputImage()->transition(cmd, VK_IMAGE_LAYOUT_GENERAL);

    pipeline->bind(cmd.get());
    pipeline->bindPushConstants(&camera->getLook(), sizeof(glm::vec3));
    pipeline->bindDescriptorSets(1, descriptorModule->getDescriptorSetPtr(0));
}

void ComputeRenderer::draw(const CommandManager &cmd) {
    pipeline->execute();
}

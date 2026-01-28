#include "ComputeRenderer.h"

#include <glm/vec3.hpp>

#include "DescriptorLayoutBuilder.h"
#include "DescriptorWriter.h"

ComputeRenderer::ComputeRenderer(const VulkanContext &vulkanContext,
                                 const Camera *camera)
    : Renderer(vulkanContext, 0, 1), camera(camera) {
    device = vulkanContext.getDevice();

    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    descriptorSetLayout = layoutBuilder.build(device, VK_SHADER_STAGE_COMPUTE_BIT, 0);

    pipeline = std::make_unique<ComputePipeline>(device, descriptorSetLayout);

    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
    };
    rendererDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(device, resourceSizes);

    descriptorSet = rendererDescriptorAllocator->allocate(descriptorSetLayout);
}

ComputeRenderer::~ComputeRenderer() {
    rendererDescriptorAllocator->resetPools();

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void ComputeRenderer::execute(CommandManager &cmd) {
    getOutput()->transition(cmd, VK_IMAGE_LAYOUT_GENERAL);

    pipeline->bind(cmd.get());
    pipeline->bindPushConstants(&camera->getLook(), sizeof(glm::vec3));
    pipeline->bindDescriptorSets(1, &descriptorSet);
    pipeline->execute();
}

void ComputeRenderer::setup(double deltaTime) {
    // This is a really silly hack to work around the fact that there's only one descriptor set
    // when there should be as many as there are swapchain images
    // TODO: Figure out a system to manage descriptor sets per-swapchain-image
    if (!firstRun) return;

    DescriptorWriter::writeImage(vulkanContext, descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                 getOutput()->getImageView(),
                                 VK_IMAGE_LAYOUT_GENERAL);

    firstRun = false;
}

#include "ComputeRenderer.h"

#include <glm/vec3.hpp>

#include "DescriptorLayoutBuilder.h"
#include "DescriptorWriter.h"

ComputeRenderer::ComputeRenderer(const VulkanContext &vulkanContext, const ImageResource &renderTarget,
                                 const Camera *camera)
    : IRenderer(vulkanContext), camera(camera) {
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

    DescriptorWriter::writeImage(vulkanContext, descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, renderTarget.imageView,
                                 VK_IMAGE_LAYOUT_GENERAL);
}

ComputeRenderer::~ComputeRenderer() {
    rendererDescriptorAllocator->resetPools();

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void ComputeRenderer::setup(ImageResource finalTarget, double deltaTime) {
    IRenderer::setup(finalTarget, deltaTime);
}

void ComputeRenderer::execute(VkCommandBuffer cmd) {
    pipeline->bind(cmd);
    pipeline->bindPushConstants(&camera->getLook(), sizeof(glm::vec3));
    pipeline->bindDescriptorSets(1, &descriptorSet);
    pipeline->execute();
}

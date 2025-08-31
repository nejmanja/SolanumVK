#include "ComputeRenderer.h"

#include "DescriptorLayoutBuilder.h"

#include <iostream>

ComputeRenderer::ComputeRenderer(const VulkanContext &vulkanContext, const ImageResource &renderTarget)
{
    device = vulkanContext.getDevice();

    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    descriptorSetLayout = layoutBuilder.build(device, VK_SHADER_STAGE_COMPUTE_BIT, 0);

    pipeline = std::make_unique<ComputePipeline>(device, descriptorSetLayout);

    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}};
    rendererDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(device, resourceSizes);

    descriptorSet = rendererDescriptorAllocator->allocate(descriptorSetLayout);

    descriptorWriter = std::make_unique<DescriptorWriter>(vulkanContext);
    descriptorWriter->writeImage(descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, renderTarget.imageView, VK_IMAGE_LAYOUT_GENERAL);
}

ComputeRenderer::~ComputeRenderer()
{
    rendererDescriptorAllocator->resetPools();

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void ComputeRenderer::setup(ImageResource finalTarget)
{
    IRenderer::setup(finalTarget);
}

void ComputeRenderer::execute(VkCommandBuffer cmd)
{
    pipeline->bind(cmd);
    pipeline->bindDescriptorSets(1, &descriptorSet);
    pipeline->execute();
}

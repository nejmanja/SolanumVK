#include "ComputeRenderer.h"

#include "DescriptorLayoutBuilder.h"

#include <iostream>

ComputeRenderer::ComputeRenderer(const VulkanContext &vulkanContext)
{
    device = vulkanContext.getDevice();

    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    auto descriptorSetLayout = layoutBuilder.build(device, VK_SHADER_STAGE_COMPUTE_BIT, 0);

    pipeline = std::make_unique<ComputePipeline>(device, descriptorSetLayout);
    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}};
    rendererDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(device, resourceSizes);

    for (int i = 0; i < vulkanContext.getSwapchain().framesInFlight; i++)
        descriptorSets.push_back(rendererDescriptorAllocator->allocate(descriptorSetLayout));
}

ComputeRenderer::~ComputeRenderer()
{
    rendererDescriptorAllocator->resetPools();
}

void ComputeRenderer::setup(SwapchainImageResource finalTarget)
{
    IRenderer::setup(finalTarget);

    std::cout << "Swapchain index: " << finalTarget.swapchainIndex << std::endl;

    VkDescriptorImageInfo imageInfo{.sampler = nullptr,
                                    .imageView = finalTarget.resource.imageView,
                                    .imageLayout = VK_IMAGE_LAYOUT_GENERAL};

    VkWriteDescriptorSet descriptorWrite = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .pNext = nullptr};
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstSet = descriptorSets[finalTarget.swapchainIndex];
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void ComputeRenderer::exec(VkCommandBuffer cmd)
{
    pipeline->bind(cmd);
    pipeline->bindDescriptorSets(1, &descriptorSets[finalTarget.swapchainIndex]);
    pipeline->execute();
}

#include "DescriptorWriter.h"

void DescriptorWriter::writeImage(const VulkanContext &vulkanContext, VkDescriptorSet descriptorSet,
                                  VkDescriptorType descriptorType, VkImageView imageView, VkImageLayout imageLayout,
                                  VkSampler sampler) {
    VkDescriptorImageInfo imageInfo{
        .sampler = sampler,
        .imageView = imageView,
        .imageLayout = imageLayout
    };

    VkWriteDescriptorSet descriptorWrite = getDefaultWrite(descriptorSet, descriptorType);
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(vulkanContext.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void DescriptorWriter::writeBuffer(const VulkanContext &vulkanContext, VkDescriptorSet descriptorSet,
                                   VkDescriptorType descriptorType, VkBuffer buffer, VkDeviceSize bufferSize,
                                   VkDeviceSize offset) {
    VkDescriptorBufferInfo bufferInfo{
        .buffer = buffer,
        .offset = offset,
        .range = bufferSize
    };

    VkWriteDescriptorSet descriptorWrite = getDefaultWrite(descriptorSet, descriptorType);
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(vulkanContext.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

VkWriteDescriptorSet DescriptorWriter::getDefaultWrite(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType) {
    return VkWriteDescriptorSet{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = descriptorType,
    };
}

#include "DescriptorWriter.h"

void DescriptorWriter::writeImage(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, VkImageView imageView, VkImageLayout imageLayout)
{
    VkDescriptorImageInfo imageInfo{.sampler = nullptr,
                                    .imageView = imageView,
                                    .imageLayout = imageLayout};

    VkWriteDescriptorSet descriptorWrite = getDefaultWrite(descriptorSet, descriptorType);
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void DescriptorWriter::writeBuffer(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, VkBuffer buffer, VkDeviceSize bufferSize, VkDeviceSize offset)
{
    VkDescriptorBufferInfo bufferInfo{
        .buffer = buffer,
        .offset = offset,
        .range = bufferSize};

    VkWriteDescriptorSet descriptorWrite = getDefaultWrite(descriptorSet, descriptorType);
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

VkWriteDescriptorSet DescriptorWriter::getDefaultWrite(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType)
{
    return VkWriteDescriptorSet{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = descriptorType,
    };
}

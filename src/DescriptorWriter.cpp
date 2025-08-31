#include "DescriptorWriter.h"

void DescriptorWriter::writeImage(VkDescriptorSet descriptorSet, VkImageView imageView, VkImageLayout imageLayout)
{
    VkDescriptorImageInfo imageInfo{.sampler = nullptr,
                                    .imageView = imageView,
                                    .imageLayout = imageLayout};

    VkWriteDescriptorSet descriptorWrite = getDefaultWrite();
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

VkWriteDescriptorSet DescriptorWriter::getDefaultWrite()
{
    return VkWriteDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = VK_NULL_HANDLE,
    };
}

#pragma once

#include "VulkanContext.h"

class DescriptorWriter
{
public:
    DescriptorWriter(const VulkanContext &vulkanContext) : device(vulkanContext.getDevice()) {}

    void writeImage(VkDescriptorSet descriptorSet, VkImageView imageView, VkImageLayout imageLayout);
    void writeBuffer(VkDescriptorSet descriptorSet, VkBuffer buffer, VkDeviceSize bufferSize, VkDeviceSize offset = 0);

private:
    static VkWriteDescriptorSet getDefaultWrite();

    VkDevice device;
};
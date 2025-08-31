#pragma once

#include "VulkanContext.h"

class DescriptorWriter
{
public:
    DescriptorWriter(const VulkanContext &vulkanContext) : device(vulkanContext.getDevice()) {}

    void writeImage(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, VkImageView imageView, VkImageLayout imageLayout);
    void writeBuffer(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, VkBuffer buffer, VkDeviceSize bufferSize, VkDeviceSize offset = 0);

private:
    static VkWriteDescriptorSet getDefaultWrite(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType);

    VkDevice device;
};
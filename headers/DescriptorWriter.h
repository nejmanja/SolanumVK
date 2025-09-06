#pragma once

#include "VulkanContext.h"

class DescriptorWriter
{
public:
    DescriptorWriter() = delete;

    static void writeImage(const VulkanContext &vulkanContext, VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, VkImageView imageView, VkImageLayout imageLayout);
    static void writeBuffer(const VulkanContext &vulkanContext, VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, VkBuffer buffer, VkDeviceSize bufferSize, VkDeviceSize offset = 0);

private:
    static VkWriteDescriptorSet getDefaultWrite(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType);
};
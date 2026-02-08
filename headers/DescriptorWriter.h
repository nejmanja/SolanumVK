#pragma once

#include "VulkanContext.h"

class DescriptorWriter {
public:
    static void writeImage(
        const VulkanContext &vulkanContext,
        VkDescriptorSet descriptorSet,
        VkDescriptorType descriptorType,
        VkImageView imageView,
        VkImageLayout imageLayout,
        VkSampler sampler = VK_NULL_HANDLE
    );

    static void writeBuffer(
        const VulkanContext &vulkanContext,
        VkDescriptorSet descriptorSet,
        VkDescriptorType descriptorType,
        VkBuffer buffer,
        VkDeviceSize bufferSize,
        VkDeviceSize offset = 0
    );

    DescriptorWriter() = delete;

private:
    static VkWriteDescriptorSet getDefaultWrite(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType);
};

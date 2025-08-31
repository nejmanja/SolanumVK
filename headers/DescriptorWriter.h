#pragma once

#include "VulkanContext.h"

class DescriptorWriter
{
public:
    DescriptorWriter(const VulkanContext &vulkanContext) : device(vulkanContext.getDevice()) {}

    void writeImage(VkDescriptorSet descriptorSet, VkImageView imageView, VkImageLayout imageLayout);

private:
    static VkWriteDescriptorSet getDefaultWrite();

    VkDevice device;
};
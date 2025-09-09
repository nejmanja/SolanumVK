#pragma once

#include "ImageResources.h"
#include "VulkanContext.h"

class ImageAllocator {
public:
    static AllocatedImageResource allocateImage2D(
        const VulkanContext &vulkanContext,
        VkFormat format,
        VkImageUsageFlags usageFlags,
        VkExtent3D extent,
        VkMemoryPropertyFlags memoryFlags,
        VmaMemoryUsage memoryUsage
    );

    static void freeImage(const VulkanContext &vulkanContext, const AllocatedImageResource &imageResource);

    ImageAllocator() = delete;

private:
};

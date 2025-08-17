#pragma once

#include "ImageResources.h"
#include "VulkanContext.h"

class ImageAllocator
{
public:
    static AllocatedImageResource allocateImage2D(const VulkanContext &vulkanContext,
                                                  const VkFormat format,
                                                  const VkImageUsageFlags usageFlags,
                                                  const VkExtent3D extent,
                                                  const VkMemoryPropertyFlags memoryFlags,
                                                  const VmaMemoryUsage memoryUsage);

    static void freeImage(const VulkanContext &vulkanContext, const AllocatedImageResource &imageResource);

private:
    ImageAllocator() = delete;
};
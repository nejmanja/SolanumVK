#pragma once

#include <ImageResources.h>

class ImageAllocator
{
public:
    static AllocatedImageResource AllocateImage2D(const VmaAllocator &allocator,
                                                  const VkFormat format,
                                                  const VkImageUsageFlags usageFlags,
                                                  const VkExtent3D extent,
                                                  const VkMemoryPropertyFlags memoryFlags,
                                                  const VmaMemoryUsage memoryUsage);

private:
    ImageAllocator() = delete;
};
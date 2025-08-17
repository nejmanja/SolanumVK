#include "ImageAllocator.h"

#include <vma/vk_mem_alloc.h>

AllocatedImageResource ImageAllocator::AllocateImage2D(const VmaAllocator &allocator,
                                                       const VkFormat format,
                                                       const VkImageUsageFlags usageFlags,
                                                       const VkExtent3D extent,
                                                       const VkMemoryPropertyFlags memoryFlags,
                                                       const VmaMemoryUsage memoryUsage)
{
    AllocatedImageResource allocatedImage{};

    VkImageCreateInfo imageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // Hard-coded to 0 for now, still very niche usage
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels = 0,                    // no mipmapping for now
        .arrayLayers = 0,                  // no image arrays here
        .samples = VK_SAMPLE_COUNT_1_BIT,  // no msaa
        .tiling = VK_IMAGE_TILING_OPTIMAL, // optimal memory tiling
        .usage = usageFlags,
        // Exclusive resource, no queue sharing
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED // == don't care
    };

    VmaAllocationCreateInfo allocationCreateInfo{
        .flags = 0,
        .usage = memoryUsage,
        .requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), // GPU-only
        .preferredFlags = 0,
        // everything else is default
    };
    allocationCreateInfo.usage = memoryUsage;

    vmaCreateImage(allocator, &imageCreateInfo, &allocationCreateInfo, &allocatedImage.resource.image, &allocatedImage.allocation, nullptr);

    return allocatedImage;
}
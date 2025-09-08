#include "ImageAllocator.h"

#include <vma/vk_mem_alloc.h>

#include "VulkanUtils.h"

AllocatedImageResource ImageAllocator::allocateImage2D(const VulkanContext &vulkanContext,
                                                       const VkFormat format,
                                                       const VkImageUsageFlags usageFlags,
                                                       const VkExtent3D extent,
                                                       const VkMemoryPropertyFlags memoryFlags,
                                                       const VmaMemoryUsage memoryUsage)
{
    AllocatedImageResource allocatedImage{};
    allocatedImage.resource.imageExtent = extent;

    VkImageCreateInfo imageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // Hard-coded to 0 for now, still very niche usage
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,                    // no mipmapping for now
        .arrayLayers = 1,                  // no image arrays here
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
        .requiredFlags = memoryFlags,
        .preferredFlags = 0,
        // everything else is default
    };

    auto imgCreateResult = vmaCreateImage(vulkanContext.getVmaAllocator(), &imageCreateInfo, &allocationCreateInfo, &allocatedImage.resource.image, &allocatedImage.allocation, nullptr);
    VulkanUtils::CheckVkResult(imgCreateResult);

    VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = allocatedImage.resource.image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {},
        .subresourceRange = {
            .aspectMask = format == VK_FORMAT_D32_SFLOAT ? (VkImageAspectFlags)VK_IMAGE_ASPECT_DEPTH_BIT : (VkImageAspectFlags)VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = 0,
            .layerCount = VK_REMAINING_ARRAY_LAYERS}};

    auto viewCreateResult = vkCreateImageView(vulkanContext.getDevice(), &imageViewCreateInfo, nullptr, &allocatedImage.resource.imageView);
    VulkanUtils::CheckVkResult(viewCreateResult);

    return allocatedImage;
}

void ImageAllocator::freeImage(const VulkanContext &vulkanContext, const AllocatedImageResource &imageResource)
{
    vkDestroyImageView(vulkanContext.getDevice(), imageResource.resource.imageView, nullptr);
    vmaDestroyImage(vulkanContext.getVmaAllocator(), imageResource.resource.image, imageResource.allocation);
}

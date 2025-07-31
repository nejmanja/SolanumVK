#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

struct ImageResource
{
    VkImage image;
    VkImageView imageView;
};

struct AllocatedImageResource
{
    ImageResource resource;
    VmaAllocation allocation;
};
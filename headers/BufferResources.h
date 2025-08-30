#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

struct AllocatedBuffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
};
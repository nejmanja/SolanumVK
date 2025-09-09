#pragma once

#include <vector>
#include <vma/vk_mem_alloc.h>

#include "VulkanContext.h"
#include "BufferResources.h"

class BufferAllocator {
public:
    static AllocatedBuffer allocateBuffer(
        const VulkanContext &vulkanContext,
        VkDeviceSize bufferSize,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocationFlags = 0
    );

    static void copyBufferData(
        const VulkanContext &vulkanContext,
        const void *cpuData,
        size_t dataSize,
        AllocatedBuffer buffer
    );

    BufferAllocator() = delete;
};

#pragma once

#include <vector>
#include <vma/vk_mem_alloc.h>

#include "VulkanContext.h"
#include "BufferResources.h"

class BufferAllocator
{
public:
    BufferAllocator(const VulkanContext &vulkanContext);
    ~BufferAllocator();

    AllocatedBuffer allocateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                   VmaAllocationCreateFlags allocationFlags = 0);

    void copyBufferData(const void *cpuData, size_t dataSize, AllocatedBuffer buffer);

private:
    std::vector<AllocatedBuffer>
        allocatedBuffers;
    VmaAllocator allocator;
};
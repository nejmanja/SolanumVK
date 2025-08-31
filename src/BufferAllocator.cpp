#include "BufferAllocator.h"

#include "VulkanUtils.h"

BufferAllocator::BufferAllocator(const VulkanContext &vulkanContext)
    : allocator(vulkanContext.getVmaAllocator())
{
}

AllocatedBuffer BufferAllocator::allocateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                                VmaAllocationCreateFlags allocationFlags)
{
    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = bufferSize,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE};

    VmaAllocationCreateInfo vmaInfo{
        .flags = allocationFlags,
        .usage = memoryUsage};

    AllocatedBuffer buffer{};

    auto result = vmaCreateBuffer(allocator, &bufferCreateInfo, &vmaInfo, &buffer.buffer, &buffer.allocation, nullptr);
    VulkanUtils::CheckVkResult(result);
    return buffer;
}

void BufferAllocator::copyBufferData(const void *cpuData, size_t dataSize, AllocatedBuffer buffer)
{
    void *data;
    auto result = vmaMapMemory(allocator, buffer.allocation, &data);
    memcpy(data, cpuData, dataSize);
    vmaUnmapMemory(allocator, buffer.allocation);
}

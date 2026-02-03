#include "BufferAllocator.h"

#include "VulkanUtils.h"

AllocatedBuffer BufferAllocator::allocateBuffer(const VulkanContext &vulkanContext, VkDeviceSize bufferSize,
                                                VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                                VmaAllocationCreateFlags allocationFlags) {
    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = bufferSize,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE
    };

    VmaAllocationCreateInfo vmaInfo{
        .flags = allocationFlags,
        .usage = memoryUsage
    };

    AllocatedBuffer buffer{};

    auto result = vmaCreateBuffer(vulkanContext.getVmaAllocator(), &bufferCreateInfo, &vmaInfo, &buffer.buffer,
                                  &buffer.allocation, &buffer.allocationInfo);
    VulkanUtils::CheckVkResult(result);
    return buffer;
}

void BufferAllocator::copyBufferData(const VulkanContext &vulkanContext,
                                     const void *cpuData, const size_t dataSize, const size_t offset,
                                     const AllocatedBuffer &buffer) {
    auto &allocator = vulkanContext.getVmaAllocator();

    void *data = buffer.allocationInfo.pMappedData;
    auto alreadyMapped = data != nullptr;
    if (!alreadyMapped)
        vmaMapMemory(allocator, buffer.allocation, &data);

    memcpy(static_cast<char *>(data) + offset, cpuData, dataSize);

    if (!alreadyMapped)
        vmaUnmapMemory(allocator, buffer.allocation);
}

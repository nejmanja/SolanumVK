#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class DescriptorSetAllocator
{
public:
    struct PoolResourceSizePerSet
    {
        VkDescriptorType type;
        // The expected (average) count of the resource of given type PER SET
        // When allocating a pool, this value will be multiplied with maxSets.
        uint32_t countPerSet;
    };

    DescriptorSetAllocator(const VkDevice device, const std::vector<PoolResourceSizePerSet> &resourceSizes);
    ~DescriptorSetAllocator();

    void resetPools();
    VkDescriptorSet allocate(const VkDescriptorSetLayout layout);

private:
    void allocateNewFreePool();
    VkDescriptorPool getPool();

    std::vector<VkDescriptorPoolSize> poolSizes;
    std::vector<VkDescriptorPool> freePools;
    std::vector<VkDescriptorPool> fullPools;

    static constexpr int maxSets = 1024;

    VkDevice device;
};
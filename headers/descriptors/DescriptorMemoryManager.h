#pragma once

#include <vector>
#include <map>
#include <vulkan/vulkan.h>


class DescriptorMemoryManager {
public:
    struct PoolResourceSizePerSet {
        VkDescriptorType type;
        // The expected (average) count of the resource of given type PER SET
        // When allocating a pool, this value will be multiplied with maxSets.
        uint32_t countPerSet;
    };

    explicit DescriptorMemoryManager(VkDevice device);

    ~DescriptorMemoryManager();

    void addBindings(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings);

    void initialize();

    void resetPools();

    VkDescriptorSet allocate(VkDescriptorSetLayout layout);

private:
    void allocateNewFreePool();

    VkDescriptorPool getPool();

    bool initialized = false;

    std::map<VkDescriptorType, uint32_t> resourceSizes{};
    std::vector<VkDescriptorPoolSize> poolSizes;
    std::vector<VkDescriptorPool> freePools;
    std::vector<VkDescriptorPool> fullPools;

    static constexpr int maxSets = 1024;

    VkDevice device{VK_NULL_HANDLE};
};

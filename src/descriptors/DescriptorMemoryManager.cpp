#include "DescriptorMemoryManager.h"

#include <cassert>

#include "VulkanUtils.h"

DescriptorMemoryManager::DescriptorMemoryManager(const VkDevice device)
    : device(device) {
}

DescriptorMemoryManager::~DescriptorMemoryManager() {
    for (auto &&pool: freePools) {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }

    for (auto &&pool: fullPools) {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
}

void DescriptorMemoryManager::addBindings(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings) {
    assert(!initialized);

    // add appropriate sizes for descriptor pools...
    for (auto binding: layoutBindings) {
        if (!resourceSizes.contains(binding.descriptorType))
            resourceSizes[binding.descriptorType] = 0;

        resourceSizes[binding.descriptorType]++;
    }
}

void DescriptorMemoryManager::initialize() {
    if (initialized) return;

    for (auto [descriptorType, resourceCount]: resourceSizes) {
        poolSizes.push_back(VkDescriptorPoolSize{.type = descriptorType, .descriptorCount = maxSets * resourceCount});
    }

    resourceSizes.clear();
    initialized = true;

    allocateNewFreePool();
}

void DescriptorMemoryManager::resetPools() {
    for (auto &&pool: freePools) {
        vkResetDescriptorPool(device, pool, 0);
    }
    for (auto &&pool: fullPools) {
        vkResetDescriptorPool(device, pool, 0);
        freePools.push_back(pool);
    }
    fullPools.clear();
}

VkDescriptorSet DescriptorMemoryManager::allocate(const VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo allocInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, .pNext = nullptr};
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    allocInfo.descriptorPool = getPool();

    VkDescriptorSet descriptorSet;
    auto result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);

    // Pool was completely used up, try again with a new pool
    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
        fullPools.push_back(allocInfo.descriptorPool);
        allocInfo.descriptorPool = getPool();

        auto result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
        VulkanUtils::CheckVkResult(result);
    }

    freePools.push_back(allocInfo.descriptorPool);

    return descriptorSet;
}

void DescriptorMemoryManager::allocateNewFreePool() {
    assert(initialized);

    VkDescriptorPoolCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, .pNext = nullptr};
    createInfo.maxSets = maxSets;
    createInfo.flags = 0;
    createInfo.poolSizeCount = (uint32_t) poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();

    VkDescriptorPool descriptorPool;
    auto result = vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool);
    VulkanUtils::CheckVkResult(result);

    freePools.push_back(descriptorPool);
}

VkDescriptorPool DescriptorMemoryManager::getPool() {
    // no pool available for alloc, create a new one
    if (freePools.size() == 0)
        allocateNewFreePool();

    auto pool = freePools.back();
    freePools.pop_back();

    return pool;
}

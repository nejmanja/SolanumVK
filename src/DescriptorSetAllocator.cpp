#include "DescriptorSetAllocator.h"

DescriptorSetAllocator::DescriptorSetAllocator(const VkDevice device, const std::vector<PoolResourceSizePerSet> &resourceSizes)
    : device(device)
{
    for (auto &&resourceSize : resourceSizes)
    {
        poolSizes.push_back(VkDescriptorPoolSize{.type = resourceSize.type, .descriptorCount = maxSets * resourceSize.countPerSet});
    }

    allocateNewFreePool();
}

DescriptorSetAllocator::~DescriptorSetAllocator()
{
    for (auto &&pool : freePools)
    {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }

    for (auto &&pool : fullPools)
    {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
}

void DescriptorSetAllocator::resetPools()
{
    for (auto &&pool : freePools)
    {
        vkResetDescriptorPool(device, pool, 0);
    }
    for (auto &&pool : fullPools)
    {
        vkResetDescriptorPool(device, pool, 0);
        freePools.push_back(pool);
    }
    fullPools.clear();
}

VkDescriptorSet DescriptorSetAllocator::allocate(const VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, .pNext = nullptr};
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    allocInfo.descriptorPool = getPool();

    VkDescriptorSet descriptorSet;
    auto result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);

    // Pool was completely used up, try again with a new pool
    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
    {
        fullPools.push_back(allocInfo.descriptorPool);
        allocInfo.descriptorPool = getPool();

        vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
    }

    freePools.push_back(allocInfo.descriptorPool);

    return descriptorSet;
}

void DescriptorSetAllocator::allocateNewFreePool()
{
    VkDescriptorPoolCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, .pNext = nullptr};
    createInfo.maxSets = maxSets;
    createInfo.flags = 0;
    createInfo.poolSizeCount = (uint32_t)poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool);

    freePools.push_back(descriptorPool);
}

VkDescriptorPool DescriptorSetAllocator::getPool()
{
    // no pool available for alloc, create a new one
    if (freePools.size() == 0)
        allocateNewFreePool();

    auto pool = freePools.back();
    freePools.pop_back();

    return pool;
}

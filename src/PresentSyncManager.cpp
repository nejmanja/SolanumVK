#include "PresentSyncManager.h"

PresentSyncManager::PresentSyncManager(VkDevice device, uint32_t frameCount)
    : device(device)
{
    VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr};
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = nullptr};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < frameCount; ++i)
    {
        PresentSyncPrimitives primitives{};
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &primitives.renderSemaphore);
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &primitives.swapchainSemaphore);
        vkCreateFence(device, &fenceInfo, nullptr, &primitives.renderFence);

        syncPrimitives.push_back(primitives);
    }
}

PresentSyncManager::~PresentSyncManager()
{
    for (auto &&primitives : syncPrimitives)
    {
        vkDestroySemaphore(device, primitives.swapchainSemaphore, nullptr);
        vkDestroySemaphore(device, primitives.renderSemaphore, nullptr);
        vkDestroyFence(device, primitives.renderFence, nullptr);
    }

    for (auto &&recycledSemaphore : recycledSemaphores)
    {
        vkDestroySemaphore(device, recycledSemaphore, nullptr);
    }
}

const VkSemaphore PresentSyncManager::getRecycledSemaphore()
{
    VkSemaphore recycledSemaphore;

    // If there are no ready semaphores, create a new one
    if (recycledSemaphores.empty())
    {
        VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr};
        semaphoreInfo.flags = 0;

        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &recycledSemaphore);
    }
    // otherwise, just get an already created one
    else
    {
        recycledSemaphore = recycledSemaphores.back();
        recycledSemaphores.pop_back();
    }

    return recycledSemaphore;
}

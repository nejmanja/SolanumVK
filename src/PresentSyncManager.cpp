#include "PresentSyncManager.h"

PresentSyncManager::PresentSyncManager(VkDevice device, uint32_t frameCount, uint32_t swapchainImageCount)
    : device(device) {
    VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr};
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = nullptr};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < frameCount; ++i) {
        PresentSyncPrimitives primitives{};
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &primitives.swapchainSemaphore);
        vkCreateFence(device, &fenceInfo, nullptr, &primitives.renderFence);

        syncPrimitives.push_back(primitives);
    }
     for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        VkSemaphore semaphore{};
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
        renderSemaphores.push_back(semaphore);
    }
}

PresentSyncManager::~PresentSyncManager() {
    for (auto &&primitives: syncPrimitives) {
        vkDestroySemaphore(device, primitives.swapchainSemaphore, nullptr);
        vkDestroyFence(device, primitives.renderFence, nullptr);
    }

    for (auto &&renderSemaphore: renderSemaphores) {
        vkDestroySemaphore(device, renderSemaphore, nullptr);
    }
}
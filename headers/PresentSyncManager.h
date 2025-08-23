#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class PresentSyncManager
{
public:
    PresentSyncManager(VkDevice device, uint32_t frameCount);
    ~PresentSyncManager();

    const VkSemaphore getRecycledSemaphore();
    void emplaceRecycledSemaphore(VkSemaphore semaphore) { recycledSemaphores.push_back(semaphore); }
    const VkSemaphore getRenderSemaphore(uint32_t frameIndex) const { return syncPrimitives[frameIndex].renderSemaphore; }
    const VkSemaphore getSwapchainImageAcuqiredSemaphore(uint32_t frameIndex) const { return syncPrimitives[frameIndex].swapchainSemaphore; }
    void setSwapchainImageAcuqiredSemaphore(uint32_t frameIndex, VkSemaphore semaphore) { syncPrimitives[frameIndex].swapchainSemaphore = semaphore; }
    VkFence getRenderFence(uint32_t frameIndex) { return syncPrimitives[frameIndex].renderFence; }

private:
    VkDevice device;
    struct PresentSyncPrimitives
    {
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence renderFence;
    };

    std::vector<PresentSyncPrimitives> syncPrimitives;
    std::vector<VkSemaphore> recycledSemaphores;
};
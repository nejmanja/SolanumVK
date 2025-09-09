#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class PresentSyncManager {
public:
    PresentSyncManager(VkDevice device, uint32_t frameCount);

    ~PresentSyncManager();

    const VkSemaphore getRecycledSemaphore();

    void emplaceRecycledSemaphore(VkSemaphore semaphore) { recycledSemaphores.push_back(semaphore); }

    [[nodiscard]] VkSemaphore getRenderSemaphore(const uint32_t frameIndex) const {
        return syncPrimitives[frameIndex].renderSemaphore;
    }

    [[nodiscard]] VkSemaphore getSwapchainImageAcquiredSemaphore(const uint32_t frameIndex) const {
        return syncPrimitives[frameIndex].swapchainSemaphore;
    }

    void setSwapchainImageAcuqiredSemaphore(const uint32_t frameIndex, VkSemaphore semaphore) {
        syncPrimitives[frameIndex].swapchainSemaphore = semaphore;
    }

    [[nodiscard]] VkFence getRenderFence(const uint32_t frameIndex) const {
        return syncPrimitives[frameIndex].renderFence;
    }

private:
    VkDevice device;

    struct PresentSyncPrimitives {
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence renderFence;
    };

    std::vector<PresentSyncPrimitives> syncPrimitives;
    std::vector<VkSemaphore> recycledSemaphores;
};

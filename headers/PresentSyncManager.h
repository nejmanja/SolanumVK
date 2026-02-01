#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class PresentSyncManager {
public:
    PresentSyncManager(VkDevice device, uint32_t frameCount, uint32_t swapchainImageCount);

    ~PresentSyncManager();


    [[nodiscard]] VkSemaphore getRenderSemaphore(const uint32_t swapchainImageIndex) const {
        return renderSemaphores[swapchainImageIndex];
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
        VkSemaphore swapchainSemaphore;
        VkFence renderFence;
    };

    std::vector<PresentSyncPrimitives> syncPrimitives;
    std::vector<VkSemaphore> renderSemaphores;
};

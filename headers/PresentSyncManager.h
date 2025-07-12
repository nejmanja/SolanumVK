#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class PresentSyncManager
{
public:
    PresentSyncManager(VkDevice device, uint32_t frameCount);
    ~PresentSyncManager();

    VkSemaphore getRenderSemaphore(uint32_t frameIndex) { return syncPrimitives[frameIndex].renderSemaphore; }
    VkSemaphore getSwapchainImageAcuqiredSemaphore(uint32_t frameIndex) { return syncPrimitives[frameIndex].swapchainSemaphore; }
    VkFence getRenderFence(uint32_t frameIndex) { return syncPrimitives[frameIndex].renderFence; }

private:
    VkDevice device;
    struct PresentSyncPrimitives
    {
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence renderFence;
    };

    std::vector<PresentSyncPrimitives> syncPrimitives;
};
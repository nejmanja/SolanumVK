#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "CommandBuffer.h"

class CommandManager {
public:
    CommandManager(VkDevice device, uint32_t queueFamily, uint32_t count);

    ~CommandManager();

    [[nodiscard]] VkCommandBuffer get() const { return buffers[frameIdx].commandBuffer; }

    void begin();

    void reset() const { vkResetCommandBuffer(get(), 0); }
    void end() const { vkEndCommandBuffer(get()); }

    void copyImage(VkImage src, VkImage dst, VkExtent2D srcSize, VkExtent2D dstSize);

    void nextFrame() { frameIdx = (frameIdx + 1) % buffers.size(); }

    void submit(VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence signalFence);

    void clearImage(VkImage image, VkClearColorValue clearValue);

private:
    VkDevice device;
    std::vector<CommandBuffer> buffers;
    uint32_t frameIdx{0};
};

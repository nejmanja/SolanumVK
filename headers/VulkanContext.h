#pragma once

#include <vma/vk_mem_alloc.h>
#include <vk-bootstrap/VkBootstrap.h>

#include "IWindowBridge.h"
#include "Swapchain.h"

class VulkanContext {
public:
    enum class QueueType {
        Graphics,
        Compute
    };

    explicit VulkanContext(IWindowBridge &window);

    ~VulkanContext();

    [[nodiscard]] const VkInstance &getInstance() const { return instance; }
    [[nodiscard]] const VmaAllocator &getVmaAllocator() const { return vmaAllocator; }
    [[nodiscard]] const Swapchain &getSwapchain() const { return swapchain; }
    [[nodiscard]] VkDevice getDevice() const { return device; }
    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }

    [[nodiscard]] const uint32_t getQueueFamily(QueueType type) const;

    [[nodiscard]] const VkQueue getQueue(QueueType type) const;

private:
    vkb::Instance createInstance(std::vector<const char *> windowExtensions);

    void createDevice(vkb::Instance vkbInstance);

    void createSwapchain(VkExtent2D windowExtent);

    void createVmaAllocator();

    VkSurfaceKHR surface;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VmaAllocator vmaAllocator;

    uint32_t computeFamily;
    VkQueue computeQueue;
    uint32_t graphicsFamily;
    VkQueue graphicsQueue;

    Swapchain swapchain;

    VkDebugUtilsMessengerEXT debugMessenger;
};

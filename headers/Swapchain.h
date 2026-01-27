#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "ImageResources.h"

struct Swapchain {
    VkSwapchainKHR swapchain{};
    std::vector<ImageResource> images{};
    uint32_t framesInFlight{};

    VkExtent3D getExtent() const { return images.size() > 0 ? images[0].getExtent() : VkExtent3D{}; }
    VkFormat getImageFormat() const { return images.size() > 0 ? images[0].getFormat() : VK_FORMAT_UNDEFINED; }
};

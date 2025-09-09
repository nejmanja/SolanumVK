#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>

class VulkanUtils {
public:
    static void CheckVkResult(const VkResult result) {
#if !_DEBUG
        // No need for these logs in release.
        return;
#endif

        if (result == VK_SUCCESS)
            return;

        std::cout << "Vk error detected" << string_VkResult(result) << std::endl;
        abort();
    }

    VulkanUtils() = delete;

private:
};

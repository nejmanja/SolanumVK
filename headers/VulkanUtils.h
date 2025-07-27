#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>

class VulkanUtils
{
public:
    static void CheckVkResult(VkResult result)
    {
        if (result == VK_SUCCESS)
            return;

        std::cout << "Vk error detected" << string_VkResult(result) << std::endl;
        abort();
    }

private:
    VulkanUtils() = delete;
};
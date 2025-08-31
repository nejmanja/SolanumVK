#include "ScopedVkMemoryManager.h"

ScopedVkMemoryManager::~ScopedVkMemoryManager()
{
    for (auto &&buffer : buffers)
    {
        vmaDestroyBuffer(context.getVmaAllocator(), buffer.buffer, buffer.allocation);
    }
    for (auto &&image : images)
    {
        vkDestroyImageView(context.getDevice(), image.resource.imageView, nullptr);
        vmaDestroyImage(context.getVmaAllocator(), image.resource.image, image.allocation);
    }
    for (auto &&descriptorSetLayout : descriptorSetLayouts)
    {
        vkDestroyDescriptorSetLayout(context.getDevice(), descriptorSetLayout, nullptr);
    }
}
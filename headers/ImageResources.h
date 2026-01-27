#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "CommandManager.h"

class ImageResource {
public:
    ImageResource(VkImage image, VkImageView imageView, VkExtent3D imageExtent, VkImageLayout imageLayout,
                  VkFormat imageFormat) {
        this->imageLayout = imageLayout;
        this->imageFormat = imageFormat;
        this->image = image;
        this->imageView = imageView;
        this->imageExtent = imageExtent;
    }

    ImageResource(VkImage image, VkImageView imageView, VkExtent2D imageExtent, VkImageLayout imageLayout,
                  VkFormat imageFormat) : ImageResource(image, imageView, {imageExtent.width, imageExtent.height, 1},
                                                        imageLayout, imageFormat) {
    }

    void transition(const CommandManager &cmdManager, VkImageLayout dstLayout);

    void blitContents(const CommandManager &cmdManager, ImageResource &destination);

    [[nodiscard]] VkImage getImage() const { return image; }
    [[nodiscard]] VkImageView getImageView() const { return imageView; }
    [[nodiscard]] VkExtent3D getExtent() const { return imageExtent; }
    [[nodiscard]] VkImageLayout getImageLayout() const { return imageLayout; }
    [[nodiscard]] VkFormat getFormat() const { return imageFormat; }

private:
    VkImage image;
    VkImageView imageView;
    VkExtent3D imageExtent{};

    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
};

struct SwapchainImageResource {
    ImageResource resource;
    uint32_t swapchainIndex{};
};

struct AllocatedImageResource {
    ImageResource resource;
    VmaAllocation allocation{};
};

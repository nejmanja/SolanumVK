#include "ImageResources.h"

void ImageResource::transition(const CommandManager &cmdManager, const VkImageLayout dstLayout) {
    VkImageMemoryBarrier2 imageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr};
    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = imageLayout;
    imageBarrier.newLayout = dstLayout;

    VkImageAspectFlags aspectMask = (dstLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
                                        ? VK_IMAGE_ASPECT_DEPTH_BIT
                                        : VK_IMAGE_ASPECT_COLOR_BIT;
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    imageBarrier.subresourceRange = subresourceRange;
    imageBarrier.image = image;

    VkDependencyInfo depInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .pNext = nullptr};
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmdManager.get(), &depInfo);

    // Finally, update the internal state to match the transition
    imageLayout = dstLayout;
}

void ImageResource::blitContents(const CommandManager &cmdManager, ImageResource &destination) {
    transition(cmdManager, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    destination.transition(cmdManager, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageBlit2 blit{.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr};
    blit.srcOffsets[1] = {static_cast<int32_t>(imageExtent.width), static_cast<int32_t>(imageExtent.height), 1};
    blit.dstOffsets[1] = {
        static_cast<int32_t>(destination.imageExtent.width), static_cast<int32_t>(destination.imageExtent.height), 1
    };
    blit.srcSubresource = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1
    };
    blit.dstSubresource = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1
    };

    VkBlitImageInfo2 blitInfo{.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
    blitInfo.srcImage = image;
    blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    blitInfo.dstImage = destination.image;
    blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    blitInfo.filter = VK_FILTER_LINEAR;
    blitInfo.regionCount = 1;
    blitInfo.pRegions = &blit;

    vkCmdBlitImage2(cmdManager.get(), &blitInfo);
}

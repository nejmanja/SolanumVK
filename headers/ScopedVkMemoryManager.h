#pragma once

#include <vector>

#include "VulkanContext.h"
#include "BufferResources.h"
#include "ImageResources.h"
#include "MeshData.h"

class ScopedVkMemoryManager {
public:
    explicit ScopedVkMemoryManager(const VulkanContext &context) : context(context) {
    }

    ~ScopedVkMemoryManager();

    void registerResource(const AllocatedBuffer &buffer) { buffers.push_back(buffer); }
    void registerResource(const AllocatedImageResource &image) { images.push_back(image); }
    void registerResource(const VkDescriptorSetLayout &layout) { descriptorSetLayouts.push_back(layout); }

    void registerResource(const MeshData &meshData) {
        buffers.push_back(meshData.getVertexBuffer());
        buffers.push_back(meshData.getIndexBuffer());
    }

    ScopedVkMemoryManager(const ScopedVkMemoryManager &other) = delete;

private:
    const VulkanContext &context;

    std::vector<AllocatedBuffer> buffers{};
    std::vector<AllocatedImageResource> images{};
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
};

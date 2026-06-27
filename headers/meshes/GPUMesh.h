#pragma once

#include "BufferResources.h"

class GPUMesh {
public:
    GPUMesh() = default;

    GPUMesh(const AllocatedBuffer &vertexBuffer, const AllocatedBuffer &indexBuffer,
            const uint32_t indexCount) : indexCount(indexCount), vertexBuffer(vertexBuffer), indexBuffer(indexBuffer) {
    }

    ~GPUMesh() = default;

    void bind(VkCommandBuffer cmd) const;

    void draw(VkCommandBuffer cmd) const;

    const AllocatedBuffer &getVertexBuffer() const { return vertexBuffer; }
    const AllocatedBuffer &getIndexBuffer() const { return indexBuffer; }

    uint32_t getIndexCount() const { return indexCount; }

private:
    uint32_t indexCount{};
    AllocatedBuffer vertexBuffer{}, indexBuffer{};
};

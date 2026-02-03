#pragma once

#include "BufferResources.h"

class GPUMesh {
public:
    GPUMesh() = default;

    GPUMesh(const AllocatedBuffer &vertexBuffer, const AllocatedBuffer &indexBuffer) {
        this->vertexBuffer = vertexBuffer;
        this->indexBuffer = indexBuffer;
    }

    ~GPUMesh() = default;

    const AllocatedBuffer &getVertexBuffer() const { return vertexBuffer; }
    const AllocatedBuffer &getIndexBuffer() const { return indexBuffer; }

private:
    AllocatedBuffer vertexBuffer{}, indexBuffer{};
};

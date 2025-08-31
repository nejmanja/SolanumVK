#pragma once

#include <vector>

#include "MeshDataFormatDescriptor.h"
#include "BufferResources.h"

class MeshData
{
public:
    MeshData() = default;

    void setFormatDescriptor(MeshDataFormatDescriptor descriptor) { formatDescriptor = descriptor; }
    const MeshDataFormatDescriptor &getFormatDescriptor() const { return formatDescriptor; }

    void setIndices(const std::vector<uint32_t> &indices) { this->indices = {indices}; }
    const std::vector<uint32_t> &getIndices() const { return indices; }

    virtual const size_t getVertexSize() const = 0;
    virtual const size_t getVertexCount() const = 0;
    virtual void setVertexCount(size_t vertexCount) { this->vertexCount = vertexCount; }

    virtual const void *getRawVertexData() const = 0;

    const AllocatedBuffer getVertexBuffer() const { return vertexBuffer; }
    const AllocatedBuffer getIndexBuffer() const { return indexBuffer; }

    void setBuffers(AllocatedBuffer vertexBuffer, AllocatedBuffer indexBuffer)
    {
        this->vertexBuffer = vertexBuffer;
        this->indexBuffer = indexBuffer;
    }

protected:
    size_t vertexCount;

private:
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;
    std::vector<uint32_t> indices;
    MeshDataFormatDescriptor formatDescriptor;
};
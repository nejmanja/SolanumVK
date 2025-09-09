#pragma once

#include <utility>
#include <vector>

#include "MeshDataFormatDescriptor.h"
#include "BufferResources.h"

class MeshData {
public:
    virtual ~MeshData() = default;

    MeshData() = default;

    void setFormatDescriptor(MeshDataFormatDescriptor descriptor) { formatDescriptor = std::move(descriptor); }
    [[nodiscard]] const MeshDataFormatDescriptor &getFormatDescriptor() const { return formatDescriptor; }

    void setIndices(const std::vector<uint32_t> &indices) { this->indices = {indices}; }
    [[nodiscard]] const std::vector<uint32_t> &getIndices() const { return indices; }

    [[nodiscard]] virtual size_t getVertexSize() const = 0;

    [[nodiscard]] virtual size_t getVertexCount() const = 0;

    virtual void setVertexCount(size_t vertexCount) { this->vertexCount = vertexCount; }

    [[nodiscard]] virtual const void *getRawVertexData() const = 0;

    [[nodiscard]] AllocatedBuffer getVertexBuffer() const { return vertexBuffer; }
    [[nodiscard]] AllocatedBuffer getIndexBuffer() const { return indexBuffer; }

    void setBuffers(const AllocatedBuffer &vertexBuffer, const AllocatedBuffer &indexBuffer) {
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

#pragma once

#include <vector>

#include "MeshDataFormatDescriptor.h"

class MeshData
{
public:
    MeshData() = default;

    void setFormatDescriptor(MeshDataFormatDescriptor descriptor) { formatDescriptor = descriptor; }
    const MeshDataFormatDescriptor &getFormatDescriptor() const { return formatDescriptor; }

    void setIndices(const std::vector<uint32_t> &indices) { this->indices = {indices}; }
    const std::vector<uint32_t> &getIndices() const { return indices; }

    virtual const size_t getVertexSize() = 0;
    virtual void setVertexCount(size_t vertexCount) { this->vertexCount = vertexCount; }

    virtual const void *getRawVertexData() const = 0;

protected:
    size_t vertexCount;

private:
    std::vector<uint32_t> indices;
    MeshDataFormatDescriptor formatDescriptor;
};
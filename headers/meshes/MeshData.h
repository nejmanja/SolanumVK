#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "BufferResources.h"
#include "GPUMesh.h"
#include "VertexAttributes.h"

class MeshData {
public:
    explicit MeshData(VertexAttributes attributes);

    ~MeshData();

    [[nodiscard]] const std::vector<uint32_t> &getIndices() const { return indices; }

    [[nodiscard]] size_t getVertexSize() const { return vertexSize; }

    [[nodiscard]] size_t getVertexCount() const { return vertexCount; }

    void setVertexCount(size_t vertexCount);

    [[nodiscard]] GPUMesh MeshData::uploadToGPU() const;


    [[nodiscard]] AllocatedBuffer getVertexBuffer() const { return vertexBuffer; }
    [[nodiscard]] AllocatedBuffer getIndexBuffer() const { return indexBuffer; }

    template<typename T>
    void setVertexAttributeData(VertexAttributes attribute, std::vector<T> data);

private:
    void calculateBindingStridesAndOffsets(VertexAttributes attribs);

    void allocateBindingBuffers();

    uint32_t vertexSize;
    uint64_t vertexCount;
    VertexAttributes attributes{};
    VertexAttributeDescriptors attributeDescriptors{};

    static constexpr uint32_t MaxBindings = 4;

    // Raw vertex data, per-binding!
    std::array<void *, MaxBindings> rawVertexBindingData{};
    // strides of individual bindings
    std::array<uint32_t, MaxBindings> bindingStrides{};
    // offsets of individual attributes inside a binding
    std::map<VertexAttributes, uint32_t> bindingOffsets{};
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;
    std::vector<uint32_t> indices;
};

template<typename T>
void MeshData::setVertexAttributeData(VertexAttributes attribute, std::vector<T> data) {
    const auto descriptor = attributeDescriptors.getDescriptor(attribute);
    const auto binding = descriptor.getBinding();
    const auto elementSize = descriptor.getSize();
    const auto stride = bindingStrides[binding];
    const auto offset = bindingOffsets[attribute];

    if (data.size() * sizeof(T) != elementSize * vertexCount) {
        std::cout << "ERROR: Provided raw data for attribute doesn't match expected size!" << std::endl;
        abort();
    }

    for (uint64_t i = 0; i < data.size(); ++i) {
        memcpy(rawVertexBindingData[i * stride + offset], data[i], elementSize);
    }
}

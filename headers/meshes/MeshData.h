#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <glm/vec3.hpp>

#include "SolanumConstants.h"
#include "VertexAttributes.h"
#include "VertexBuffers.h"

class MeshData {
public:
    explicit MeshData(VertexAttributes attributes);

    ~MeshData() = default;

    void setIndices(const std::vector<uint32_t> &indices) { this->indices = {indices}; }
    [[nodiscard]] const std::vector<uint32_t> &getIndices() const { return indices; }

    [[nodiscard]] size_t getVertexSize() const { return vertexSize; }

    [[nodiscard]] size_t getVertexCount() const { return vertexCount; }

    void setVertexCount(size_t vertexCount);

    [[nodiscard]] const VertexBuffer &getRawVertexBindingData() const;

    template<typename T>
    void setVertexAttributeData(VertexAttributes attribute, std::vector<T> data);

    // void setVertexAttributeData(VertexAttributes attribute, std::vector<glm::vec3> data);

    [[nodiscard]] uint32_t getNumBindings() const { return numBindings; }

private:
    void calculateBindingStridesAndOffsets(VertexAttributes attribs);

    void allocateBindingBuffers();

    uint32_t numBindings{};
    uint32_t vertexSize{};
    uint64_t vertexCount{};
    VertexAttributes attributes{};
    VertexAttributeDescriptors attributeDescriptors{};

    // TODO: Move all of this stuff into VertexBuffer!
    // Raw vertex data, per-binding!
    VertexBuffer rawVertexBuffer{};
    // strides of individual bindings
    std::array<uint32_t, SolVK::maxBindings> bindingStrides{};
    // offsets of individual attributes inside a binding
    std::map<VertexAttributes, uint32_t> bindingOffsets{};
    std::vector<uint32_t> indices{};
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

    auto *buffer = static_cast<uint8_t *>(rawVertexBuffer.getData(binding));

    for (uint64_t i = 0; i < data.size(); ++i) {
        memcpy(buffer + i * stride + offset, &data[i], elementSize);
    }
}

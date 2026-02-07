#pragma once

#include <map>
#include <vector>

#include "VertexAttributes.h"
#include "VertexBuffers.h"

class MeshData {
public:
    // Constructs a mesh with all provided attributes in binding 0
    explicit MeshData(VertexAttributes attributes);

    ~MeshData() = default;

    void setIndices(const std::vector<uint32_t> &indices) { this->indices = {indices}; }
    [[nodiscard]] const std::vector<uint32_t> &getIndices() const { return indices; }

    [[nodiscard]] size_t getVertexCount() const { return vertexCount; }

    void setVertexCount(size_t vertexCount);

    [[nodiscard]] const VertexBufferDescriptor &getVertexBuffer(uint32_t binding) const;

    template<typename T>
    void setVertexAttributeData(VertexAttributes attribute, std::vector<T> data);

    [[nodiscard]] uint32_t getNumBindings() const { return vertexBuffers.size(); }

    [[nodiscard]] std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const;

private:
    uint64_t vertexCount{};
    VertexAttributes attributes{};

    // Raw vertex data, per-binding!
    std::map<VertexAttributes, uint32_t> attributeBindings{};
    std::vector<VertexBufferDescriptor> vertexBuffers{};
    std::vector<uint32_t> indices{};
};

template<typename T>
void MeshData::setVertexAttributeData(VertexAttributes attribute, std::vector<T> data) {
    const auto binding = attributeBindings[attribute];
    vertexBuffers[binding].setVertexAttributeData(attribute, data);
}

#pragma once
#include <cstdint>
#include <utility>
#include <map>

#include "VertexAttributeDescriptors.h"

// Describes a buffer for a single vertex binding
class VertexBufferDescriptor {
public:
    VertexBufferDescriptor() = default;

    explicit VertexBufferDescriptor(VertexAttributeDescriptors attributeDescriptors)
        : attributeDescriptors(std::move(attributeDescriptors)) {
        calculateStrideAndOffsets();
    }

    ~VertexBufferDescriptor() {
        if (data != nullptr) {
            ::operator delete(data);
            data = nullptr;
        }
    }

    void setVertexCount(const uint64_t vertexCount) {
        this->vertexCount = vertexCount;
        if (data != nullptr) {
            ::operator delete(data);
            data = nullptr;
        }

        data = ::operator new(vertexCount * stride);
    }

    [[nodiscard]] uint64_t getDataSize() const { return vertexCount * stride; }
    [[nodiscard]] uint64_t getStride() const { return stride; }

    [[nodiscard]] void *getData() const { return data; }

    [[nodiscard]] const VertexAttributeDescriptor &getDescriptor(VertexAttributes attribute) const {
        return attributeDescriptors.getDescriptor(attribute);
    }

    template<typename T>
    void setVertexAttributeData(VertexAttributes attribute, std::vector<T>);

    [[nodiscard]] uint32_t getOffset(VertexAttributes attribute) const { return attributeOffsets.at(attribute); }

private:
    void calculateStrideAndOffsets();

    void *data{nullptr};
    uint64_t vertexCount{0};
    uint32_t stride{0};
    VertexAttributeDescriptors attributeDescriptors{};
    // offsets of individual attributes inside a binding
    std::map<VertexAttributes, uint32_t> attributeOffsets{};
};

template<typename T>
void VertexBufferDescriptor::setVertexAttributeData(VertexAttributes attribute, std::vector<T> vectorData) {
    const auto offset = attributeOffsets[attribute];
    const auto &descriptor = getDescriptor(attribute);

    if (vectorData.size() * sizeof(T) != descriptor.getSize() * vertexCount) {
        throw std::out_of_range("Vertex attribute data size does not match size of buffer!");
    }

    auto *buffer = static_cast<uint8_t *>(data);

    for (uint64_t i = 0; i < vectorData.size(); ++i) {
        memcpy(buffer + i * stride + offset, &vectorData[i], descriptor.getSize());
    }
}

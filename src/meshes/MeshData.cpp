#include "MeshData.h"

#include "BufferAllocator.h"

void MeshData::setVertexCount(const uint64_t count) {
    if (vertexCount == 0) {
        vertexCount = count;

        for (auto &vertexBuffer: vertexBuffers) {
            vertexBuffer.setVertexCount(vertexCount);
        }
    }
}

const VertexBufferDescriptor &MeshData::getVertexBuffer(uint32_t binding) const {
    if (binding >= vertexBuffers.size())
        throw std::out_of_range("Vertex buffer binding not valid!");

    return vertexBuffers[binding];
}


std::vector<VkVertexInputAttributeDescription> MeshData::getAttributeDescriptions() const {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    uint32_t locations[NumVertexAttributes] = {0};

    for (VertexAttributes attribute = VertexAttributes::Position;
         attribute != MaxValue;
         attribute = static_cast<VertexAttributes>(static_cast<uint32_t>(attribute) << 1)) {
        if ((attributes & attribute) == VertexAttributes::None) continue;

        auto binding = attributeBindings.at(attribute);

        auto &descriptor = vertexBuffers[binding].getDescriptor(attribute);
        auto offset = vertexBuffers[binding].getOffset(attribute);

        attributeDescriptions.push_back({
            .location = locations[binding]++,
            .binding = binding,
            .format = descriptor.getFormat(),
            .offset = offset
        });
    }

    return attributeDescriptions;
}

MeshData::MeshData(const VertexAttributes attributes) : attributes(attributes) {
    vertexBuffers.emplace_back(VertexAttributeDescriptors(attributes));

    for (VertexAttributes attribute = VertexAttributes::Position;
         attribute != MaxValue;
         attribute = static_cast<VertexAttributes>(static_cast<uint32_t>(attribute) << 1)) {
        if (hasAllAttributes(attributes, attribute)) {
            attributeBindings[attribute] = 0;
        }
    }
}

#include "MeshData.h"

#include "BufferAllocator.h"

void MeshData::setVertexCount(const uint64_t count) {
    if (vertexCount == 0) {
        vertexCount = count;
        allocateBindingBuffers();
    }
}

const VertexBuffer &MeshData::getRawVertexBindingData() const {
    return rawVertexBuffer;
}


void MeshData::calculateBindingStridesAndOffsets(VertexAttributes attribs) {
    vertexSize = 0;
    for (uint32_t binding = 0; binding < SolVK::maxBindings; binding++) {
        bindingStrides[binding] = 0;
    }

    numBindings = 0;
    for (VertexAttributes attribute = VertexAttributes::Position;
         attribute != MaxValue;
         attribute = static_cast<VertexAttributes>(static_cast<uint32_t>(attribute) << 1)) {
        // if the attribute is actually present
        if (hasAllAttributes(attribs, attribute)) {
            const auto descriptor = attributeDescriptors.getDescriptor(attribute);
            const auto binding = descriptor.getBinding();
            const auto attributeSize = descriptor.getSize();

            // The offset of individual attributes within a binding should be the size of that binding so far
            // It is implied that the ordering of attributes inside a binding is the same as the enum order
            bindingOffsets[attribute] = bindingStrides[binding];
            // Update strides per-binding, and total vertex size, which is just the sum of all of these
            bindingStrides[binding] += attributeSize;
            vertexSize += attributeSize;

            if (numBindings < binding + 1) numBindings = binding + 1;
        }
    }
}

void MeshData::allocateBindingBuffers() {
    for (uint32_t binding = 0; binding < numBindings; binding++) {
        if (bindingStrides[binding] > 0) {
            rawVertexBuffer.setDataSize(binding, bindingStrides[binding] * vertexCount);
        }
    }
}

MeshData::MeshData(const VertexAttributes attributes) : attributes(attributes) {
    calculateBindingStridesAndOffsets(attributes);
}

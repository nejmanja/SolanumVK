#include "MeshData.h"

#include <iostream>

void MeshData::setVertexCount(const uint64_t count) {
    if (vertexCount == 0) {
        vertexCount = count;
        allocateBindingBuffers();
    }
}

GPUMesh MeshData::uploadToGPU() const {
    // concatenate all bindings
    // send them into a VkBuffer
    // send indices into another buffer
    return GPUMesh{};
}


void MeshData::calculateBindingStridesAndOffsets(VertexAttributes attribs) {
    vertexSize = 0;
    for (uint32_t binding = 0; binding < MaxBindings; binding++) {
        bindingStrides[binding] = 0;
    }

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
        }
    }
}

void MeshData::allocateBindingBuffers() {
    for (uint32_t i = 0; i < MaxBindings; i++) {
        if (bindingStrides[i] > 0) {
            rawVertexBindingData[i] = ::operator new(bindingStrides[i] * vertexCount);
        } else {
            rawVertexBindingData[i] = nullptr;
        }
    }
}

MeshData::MeshData(const VertexAttributes attributes) : attributes(attributes) {
    calculateBindingStridesAndOffsets(attributes);
}

MeshData::~MeshData() {
    for (uint32_t i = 0; i < MaxBindings; i++) {
        if (bindingStrides[i] > 0) {
            ::operator delete(rawVertexBindingData[i]);
        }
    }
}

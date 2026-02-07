#include "VertexBuffers.h"

void VertexBufferDescriptor::calculateStrideAndOffsets() {
    const auto attributes = attributeDescriptors.getAttributes();

    for (VertexAttributes attribute = VertexAttributes::Position;
         attribute != MaxValue;
         attribute = static_cast<VertexAttributes>(static_cast<uint32_t>(attribute) << 1)) {
        // if the attribute is actually present
        if (hasAllAttributes(attributes, attribute)) {
            const auto descriptor = attributeDescriptors.getDescriptor(attribute);
            const auto attributeSize = descriptor.getSize();

            // The offset of individual attributes within a binding should be the size of that binding so far
            // It is implied that the ordering of attributes inside a binding is the same as the enum order
            attributeOffsets[attribute] = stride;
            // Update stride
            stride += attributeSize;
        }
    }
}

#include "VertexAttributes.h"

VertexAttributeDescriptors::VertexAttributeDescriptors() {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        auto descriptor = VertexAttributeDescriptor{attribute, VertexAttributePrecision::Float, 2, 0};
        descriptors.push_back(descriptor);
    }

    // binding 0 - position, normal, tangent (all most likely transform in VS)
    const auto binding0Attribs = VertexAttributes::Position | VertexAttributes::Normal | VertexAttributes::Tangent;
    setBinding(binding0Attribs, 0);
    setDimensionality(VertexAttributes::Position | VertexAttributes::Normal, 3);
    setDimensionality(VertexAttributes::Tangent, 4);

    // binding 1 - UVs, Color
    const auto binding1Attribs = VertexAttributes::Color | VertexAttributes::UV0 | VertexAttributes::UV1;
    setBinding(binding1Attribs, 1);
    setDimensionality(VertexAttributes::UV0 | VertexAttributes::UV1, 2);
    setDimensionality(VertexAttributes::Color, 4); // RGBA
}

void VertexAttributeDescriptors::setBinding(VertexAttributes attributes, uint32_t binding) {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        if (hasAllAttributes(attributes, attribute)) {
            descriptors[i].setBinding(binding);
        }
    }
}

void VertexAttributeDescriptors::setPrecision(VertexAttributes attributes, VertexAttributePrecision precision) {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        if (hasAllAttributes(attributes, attribute)) {
            descriptors[i].setPrecision(precision);
        }
    }
}

void VertexAttributeDescriptors::setDimensionality(VertexAttributes attributes, uint8_t dimensionality) {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        if (hasAllAttributes(attributes, attribute)) {
            descriptors[i].setDimensionality(dimensionality);
        }
    }
}

#include "VertexAttributes.h"

VkFormat VertexAttributeDescriptor::getFormat() const {
    switch (precision) {
        case VertexAttributePrecision::Half:
            switch (dimensionality) {
                case 1:
                    return VK_FORMAT_R16_SFLOAT;
                case 2:
                    return VK_FORMAT_R16G16_SFLOAT;
                case 3:
                    return VK_FORMAT_R16G16B16_SFLOAT;
                case 4:
                    return VK_FORMAT_R16G16B16A16_SFLOAT;
                default:
                    throw std::invalid_argument("Unsupported dimensionality!");
            }
        case VertexAttributePrecision::Float:
            switch (dimensionality) {
                case 1:
                    return VK_FORMAT_R32_SFLOAT;
                case 2:
                    return VK_FORMAT_R32G32_SFLOAT;
                case 3:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                default:
                    throw std::invalid_argument("Unsupported dimensionality!");
            }
        case VertexAttributePrecision::Double:
            switch (dimensionality) {
                case 1:
                    return VK_FORMAT_R64_SFLOAT;
                case 2:
                    return VK_FORMAT_R64G64_SFLOAT;
                case 3:
                    return VK_FORMAT_R64G64B64_SFLOAT;
                case 4:
                    return VK_FORMAT_R64G64B64A64_SFLOAT;
                default:
                    throw std::invalid_argument("Unsupported dimensionality!");
            }
        case VertexAttributePrecision::None:
            throw std::invalid_argument("VertexAttributePrecision::None");
        default:
            throw std::invalid_argument("Unsupported VertexAttributePrecision!");
    }
}

VertexAttributeDescriptors::VertexAttributeDescriptors() {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        auto descriptor = VertexAttributeDescriptor{attribute, VertexAttributePrecision::Float, 2, 0};
        descriptors.push_back(descriptor);
    }

    // binding 0 - position, normal, tangent (all most likely transform in VS)
    const auto binding0Attribs = VertexAttributes::Position | VertexAttributes::Normal | VertexAttributes::Tangent |
                                 VertexAttributes::Color;
    setBinding(binding0Attribs, 0);
    setDimensionality(VertexAttributes::Position | VertexAttributes::Normal, 3);
    setDimensionality(VertexAttributes::Tangent, 4);
    setDimensionality(VertexAttributes::Color, 3); // RGB

    // binding 1 - UVs
    const auto binding1Attribs = VertexAttributes::UV0 | VertexAttributes::UV1;
    setBinding(binding1Attribs, 1);
    setDimensionality(VertexAttributes::UV0 | VertexAttributes::UV1, 2);
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

VkFormat VertexAttributeDescriptors::getFormat(VertexAttributes attribute) const {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attrib = static_cast<VertexAttributes>(1 << i);
        if (hasAllAttributes(attribute, attrib)) {
            return descriptors[i].getFormat();
        }
    }

    throw std::invalid_argument("Invalid Vertex Attribute!");
}

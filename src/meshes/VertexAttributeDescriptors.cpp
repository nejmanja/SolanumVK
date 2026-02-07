#include "VertexAttributeDescriptors.h"

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

VertexAttributeDescriptors::VertexAttributeDescriptors() : attributes(
    VertexAttributes::Position | VertexAttributes::Normal | VertexAttributes::Tangent | VertexAttributes::Color |
    VertexAttributes::UV0 | VertexAttributes::UV1) {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        auto descriptor = VertexAttributeDescriptor{attribute, VertexAttributePrecision::Float, 2};
        descriptors.push_back(descriptor);
    }
    setDimensionality(VertexAttributes::Position | VertexAttributes::Normal, 3);
    setDimensionality(VertexAttributes::Tangent, 4);
    setDimensionality(VertexAttributes::Color, 3); // RGB
    setDimensionality(VertexAttributes::UV0 | VertexAttributes::UV1, 2);
}

VertexAttributeDescriptors::VertexAttributeDescriptors(VertexAttributes attributes) : attributes(attributes) {
    for (int i = 0; i < NumVertexAttributes; i++) {
        const auto attribute = static_cast<VertexAttributes>(1 << i);
        if ((attribute & attributes) != VertexAttributes::None) {
            auto descriptor = VertexAttributeDescriptor{attribute, VertexAttributePrecision::Float, 2};
            descriptors.push_back(descriptor);
        }
    }

    setDimensionality(VertexAttributes::Position | VertexAttributes::Normal, 3);
    setDimensionality(VertexAttributes::Tangent, 4);
    setDimensionality(VertexAttributes::Color, 3); // RGB
    setDimensionality(VertexAttributes::UV0 | VertexAttributes::UV1, 2);
}

VertexAttributeDescriptors::VertexAttributeDescriptors(std::vector<VertexAttributeDescriptor> descriptors)
    : descriptors(std::move(descriptors)) {
    attributes = VertexAttributes::None;

    for (auto descriptor: descriptors) {
        attributes |= descriptor.getAttribute();
    }
}

void VertexAttributeDescriptors::setPrecision(VertexAttributes attribs, VertexAttributePrecision precision) {
    if ((attribs & attributes) == VertexAttributes::None) return;

    for (auto &descriptor: descriptors) {
        if ((descriptor.getAttribute() & attribs) != VertexAttributes::None) {
            descriptor.setPrecision(precision);
        }
    }
}

void VertexAttributeDescriptors::setDimensionality(VertexAttributes attribs, uint8_t dimensionality) {
    if ((attribs & attributes) == VertexAttributes::None) return;

    for (auto &descriptor: descriptors) {
        if ((descriptor.getAttribute() & attribs) != VertexAttributes::None) {
            descriptor.setDimensionality(dimensionality);
        }
    }
}

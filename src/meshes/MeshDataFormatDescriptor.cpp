#include "MeshDataFormatDescriptor.h"

void VertexBinding::addAttribute(const VertexAttribute::VertexAttributeType type, const VkFormat format) {
    const VertexAttribute attribute{
        .type = type,
        .attributeDescription = {
            .location = static_cast<uint32_t>(attributes.size()),
            .binding = index,
            .format = format,
            .offset = totalStride
        }
    };

    updateStride(format);

    attributes.push_back(attribute);
}

VkVertexInputBindingDescription VertexBinding::createBindingDescription() const {
    return VkVertexInputBindingDescription{
        .binding = index,
        .stride = totalStride,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
}

void VertexBinding::updateStride(const VkFormat format) {
    switch (format) {
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            totalStride += 4 * sizeof(float);
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            totalStride += 3 * sizeof(float);
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            totalStride += 2 * sizeof(float);
            break;
        default:
            break;
    }
}

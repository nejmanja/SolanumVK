#pragma once

#include <utility>
#include <vector>
#include <fastgltf/core.hpp>

#include "VertexAttributes.h"

class VertexAttributeDescriptor {
public:
    VertexAttributeDescriptor(const VertexAttributes attribute, const VertexAttributePrecision precision,
                              const uint8_t dimensionality)
        : attribute(attribute),
          precision(precision),
          dimensionality(dimensionality) {
    }

    [[nodiscard]] VertexAttributes getAttribute() const {
        return attribute;
    }

    [[nodiscard]] VertexAttributePrecision getPrecision() const {
        return precision;
    }

    [[nodiscard]] uint8_t getDimensionality() const {
        return dimensionality;
    }

    [[nodiscard]] uint32_t getSize() const {
        uint32_t size = 0;
        switch (precision) {
            case VertexAttributePrecision::Half:
                size = 2;
                break;
            case VertexAttributePrecision::Float:
                size = 4;
                break;
            case VertexAttributePrecision::Double:
                size = 8;
                break;
            default:
                break;
        }
        return size * dimensionality;
    }

    void setPrecision(VertexAttributePrecision precision) {
        this->precision = precision;
    }

    void setDimensionality(uint8_t dimensionality) {
        this->dimensionality = dimensionality;
    }

    VkFormat getFormat() const;

private:
    VertexAttributes attribute;
    VertexAttributePrecision precision;
    uint8_t dimensionality;
};

class VertexAttributeDescriptors {
public:
    // Generates an assumed layout for all attributes.
    // If required, the layout may be tweaked
    VertexAttributeDescriptors();

    explicit VertexAttributeDescriptors(VertexAttributes attributes);

    explicit VertexAttributeDescriptors(std::vector<VertexAttributeDescriptor> descriptors);

    void setPrecision(VertexAttributes attributes, VertexAttributePrecision precision);

    void setDimensionality(VertexAttributes attribs, uint8_t dimensionality);

    [[nodiscard]] VertexAttributes getAttributes() const { return attributes; }

    [[nodiscard]] const VertexAttributeDescriptor &getDescriptor(VertexAttributes attribute) const {
        for (const auto &descriptor: descriptors) {
            if (descriptor.getAttribute() == attribute) {
                return descriptor;
            }
        }

        throw std::runtime_error("Vertex attribute descriptor not found!");
    }

private:
    VertexAttributes attributes;
    std::vector<VertexAttributeDescriptor> descriptors{};
};

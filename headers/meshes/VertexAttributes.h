#pragma once
#include <cstdint>
#include <vector>
#include <fastgltf/core.hpp>
#include <vulkan/vulkan_core.h>

enum class VertexAttributePrecision {
    None = 0,
    Half = 1,
    Float = 2,
    Double = 3,
};

enum class VertexAttributes : uint32_t {
    None = 0,
    Position = 1 << 0,
    Normal = 1 << 1,
    Tangent = 1 << 2,
    Color = 1 << 3,
    UV0 = 1 << 4,
    UV1 = 1 << 5,
};

constexpr auto MaxValue = VertexAttributes::UV1;
constexpr int NumVertexAttributes = 6;

class VertexAttributeDescriptor {
public:
    VertexAttributeDescriptor(const VertexAttributes attribute, const VertexAttributePrecision precision,
                              const uint8_t dimensionality, const uint32_t binding)
        : attribute(attribute),
          precision(precision),
          dimensionality(dimensionality),
          binding(binding) {
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

    [[nodiscard]] uint32_t getBinding() const {
        return binding;
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

    void setBinding(uint32_t binding) {
        this->binding = binding;
    }

    VkFormat getFormat() const;

private:
    VertexAttributes attribute;
    VertexAttributePrecision precision;
    uint8_t dimensionality;
    uint32_t binding;
};

class VertexAttributeDescriptors {
public:
    // Generates an assumed layout for all attributes.
    // If required, the layout may be tweaked
    VertexAttributeDescriptors();

    // Set the binding for the given attributes
    void setBinding(VertexAttributes attributes, uint32_t binding);

    void setPrecision(VertexAttributes attributes, VertexAttributePrecision precision);

    void setDimensionality(VertexAttributes attributes, uint8_t dimensionality);

    VkFormat getFormat(VertexAttributes attribute) const;

    [[nodiscard]] const VertexAttributeDescriptor &getDescriptor(VertexAttributes attribute) const {
        for (const auto &descriptor: descriptors) {
            if (descriptor.getAttribute() == attribute) {
                return descriptor;
            }
        }

        throw std::runtime_error("Vertex attribute descriptor not found!");
    }

private:
    std::vector<VertexAttributeDescriptor> descriptors;
};

inline VertexAttributes operator|(VertexAttributes a, VertexAttributes b) {
    return static_cast<VertexAttributes>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline VertexAttributes operator&(VertexAttributes a, VertexAttributes b) {
    return static_cast<VertexAttributes>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline VertexAttributes operator~(VertexAttributes a) {
    return static_cast<VertexAttributes>(~static_cast<uint32_t>(a));
}

inline VertexAttributes &operator|=(VertexAttributes &a, const VertexAttributes b) {
    a = a | b;
    return a;
}

inline VertexAttributes &operator&=(VertexAttributes &a, const VertexAttributes b) {
    a = a & b;
    return a;
}

inline bool hasAllAttributes(const VertexAttributes attributes, const VertexAttributes attributesToTest) {
    return (attributes & attributesToTest) != VertexAttributes::None;
}

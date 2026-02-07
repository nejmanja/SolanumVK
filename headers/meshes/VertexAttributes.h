#pragma once
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

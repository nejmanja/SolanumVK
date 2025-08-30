#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

class VertexAttribute
{
public:
    enum class VertexAttributeType
    {
        Position,
        Normal,
        Tangent,
        Color,
        UV0,
        UV1,
    };
    VertexAttributeType type;
    VkVertexInputAttributeDescription attributeDescription;
};

class VertexBinding
{
public:
    VertexBinding(uint32_t index) : index(index) {}
    void addAttribute(VertexAttribute::VertexAttributeType type, VkFormat format);

    const VkVertexInputBindingDescription createBindingDescription() const;

    const uint32_t getIndex() const { return index; }
    const uint32_t getStride() const { return totalStride; }
    const std::vector<VertexAttribute> &getAttributes() const { return attributes; }

private:
    void updateStride(VkFormat format);

    uint32_t totalStride{0};
    uint32_t index{0};
    std::vector<VertexAttribute> attributes{};
};

class MeshDataFormatDescriptor
{
public:
    MeshDataFormatDescriptor() = default;

    const std::vector<std::string> getExpectedGltfAttributes() { return std::vector<std::string>{}; }
    const std::vector<VertexBinding> getBindingDescriptors() const { return bindings; }

    void addBinding(VertexBinding binding) { bindings.push_back(binding); }

private:
    std::vector<VertexBinding> bindings{};
};
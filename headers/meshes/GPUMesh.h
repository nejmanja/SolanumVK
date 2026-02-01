#pragma once

#include <vulkan/vulkan.h>

class GPUMesh {
public:
    GPUMesh() = default;

private:
    VkBuffer vertexBuffer{}, indexBuffer{};
};

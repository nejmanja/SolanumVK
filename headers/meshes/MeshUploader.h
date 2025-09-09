#pragma once

#include "MeshData.h"
#include "VulkanContext.h"

class MeshUploader {
public:
    static void uploadMesh(const VulkanContext &vulkanContext, MeshData &meshData);

    MeshUploader() = delete;
};

#pragma once

#include "GPUMesh.h"
#include "MeshData.h"
#include "VulkanContext.h"

class MeshUploader {
public:
    static GPUMesh uploadMesh(const VulkanContext &vulkanContext, const MeshData *meshData);

    MeshUploader() = delete;
};

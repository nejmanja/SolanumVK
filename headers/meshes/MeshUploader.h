#pragma once

#include <vma/vk_mem_alloc.h>

#include "VulkanContext.h"
#include "MeshData.h"
#include "BufferAllocator.h"

class MeshUploader
{
public:
    static void uploadMesh(const VulkanContext &vulkanContext, MeshData &meshData);

private:
    MeshUploader() = delete;
};
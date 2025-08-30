#pragma once

#include <vma/vk_mem_alloc.h>

#include "VulkanContext.h"
#include "MeshData.h"
#include "BufferAllocator.h"

class MeshUploader
{
public:
    MeshUploader(const VulkanContext &vulkanContext);
    ~MeshUploader() {}

    void uploadMesh(MeshData &meshData);

private:
    BufferAllocator allocator;
};
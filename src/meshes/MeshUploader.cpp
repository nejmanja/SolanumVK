#include "MeshUploader.h"

MeshUploader::MeshUploader(const VulkanContext &vulkanContext) : allocator(vulkanContext)
{
}

void MeshUploader::uploadMesh(MeshData &meshData)
{
    auto vertexBufferSize = meshData.getVertexSize() * meshData.getVertexCount();
    auto indexBufferSize = meshData.getIndices().size() * sizeof(uint32_t);

    auto vertexBuffer = allocator.allocateBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    allocator.copyBufferData(meshData.getRawVertexData(), vertexBufferSize, vertexBuffer);

    auto indexBuffer = allocator.allocateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    allocator.copyBufferData(meshData.getIndices().data(), indexBufferSize, indexBuffer);

    meshData.setBuffers(vertexBuffer, indexBuffer);
}
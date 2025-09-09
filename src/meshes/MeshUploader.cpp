#include "MeshUploader.h"

#include "BufferAllocator.h"

void MeshUploader::uploadMesh(const VulkanContext &vulkanContext, MeshData &meshData) {
    auto vertexBufferSize = meshData.getVertexSize() * meshData.getVertexCount();
    auto indexBufferSize = meshData.getIndices().size() * sizeof(uint32_t);

    auto vertexBuffer = BufferAllocator::allocateBuffer(vulkanContext, vertexBufferSize,
                                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    BufferAllocator::copyBufferData(vulkanContext, meshData.getRawVertexData(), vertexBufferSize, vertexBuffer);

    auto indexBuffer = BufferAllocator::allocateBuffer(vulkanContext, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                       VMA_MEMORY_USAGE_CPU_TO_GPU);
    BufferAllocator::copyBufferData(vulkanContext, meshData.getIndices().data(), indexBufferSize, indexBuffer);

    meshData.setBuffers(vertexBuffer, indexBuffer);
}

#include "MeshUploader.h"

#include "BufferAllocator.h"

GPUMesh MeshUploader::uploadMesh(const VulkanContext &vulkanContext, const MeshData *meshData) {
    const auto vertexBufferSize = meshData->getVertexBuffer(0).getDataSize();
    const auto indexBufferSize = meshData->getIndices().size() * sizeof(uint32_t);

    const auto vertexBuffer = BufferAllocator::allocateBuffer(vulkanContext, vertexBufferSize,
                                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                              VMA_MEMORY_USAGE_CPU_TO_GPU);

    uint64_t offset = 0;
    for (uint32_t binding = 0; binding < meshData->getNumBindings(); binding++) {
        const auto &buffer = meshData->getVertexBuffer(binding);
        const auto *data = buffer.getData();
        BufferAllocator::copyBufferData(vulkanContext, data, buffer.getDataSize(), offset, vertexBuffer);
        offset += buffer.getDataSize();
    }

    const auto indexBuffer = BufferAllocator::allocateBuffer(vulkanContext, indexBufferSize,
                                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                             VMA_MEMORY_USAGE_CPU_TO_GPU);
    BufferAllocator::copyBufferData(vulkanContext, meshData->getIndices().data(), indexBufferSize, 0, indexBuffer);

    return {vertexBuffer, indexBuffer};
}

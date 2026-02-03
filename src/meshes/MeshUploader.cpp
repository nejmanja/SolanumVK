#include "MeshUploader.h"

#include "BufferAllocator.h"

GPUMesh MeshUploader::uploadMesh(const VulkanContext &vulkanContext, const MeshData *meshData) {
    const auto vertexBufferSize = meshData->getVertexSize() * meshData->getVertexCount();
    const auto indexBufferSize = meshData->getIndices().size() * sizeof(uint32_t);

    const auto vertexBuffer = BufferAllocator::allocateBuffer(vulkanContext, vertexBufferSize,
                                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                              VMA_MEMORY_USAGE_CPU_TO_GPU);

    uint64_t offset = 0;
    const auto &bindingData = meshData->getRawVertexBindingData();
    for (uint32_t binding = 0; binding < meshData->getNumBindings(); binding++) {
        const auto *data = bindingData.getData(binding);
        BufferAllocator::copyBufferData(vulkanContext, data, bindingData.getDataSize(binding), offset, vertexBuffer);
        offset += bindingData.getDataSize(binding);
    }

    const auto indexBuffer = BufferAllocator::allocateBuffer(vulkanContext, indexBufferSize,
                                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                             VMA_MEMORY_USAGE_CPU_TO_GPU);
    BufferAllocator::copyBufferData(vulkanContext, meshData->getIndices().data(), indexBufferSize, 0, indexBuffer);

    return GPUMesh(vertexBuffer, indexBuffer);
}

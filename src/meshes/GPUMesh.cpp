#include "GPUMesh.h"

void GPUMesh::bind(VkCommandBuffer cmd) const {
    static constexpr VkDeviceSize offset{0};

    // TODO: Add support for multiple streams
    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}

void GPUMesh::draw(VkCommandBuffer cmd) const {
    vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}

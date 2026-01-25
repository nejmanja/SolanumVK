#include "meshes/UVMappedMeshData.h"

UVMappedMeshData::UVMappedMeshData() {
    MeshDataFormatDescriptor formatDescriptor{};

    VertexBinding defaultBinding{0};
    defaultBinding.addAttribute(VertexAttribute::VertexAttributeType::Position, VK_FORMAT_R32G32B32_SFLOAT);
    defaultBinding.addAttribute(VertexAttribute::VertexAttributeType::Normal, VK_FORMAT_R32G32B32_SFLOAT);
    VertexBinding uvBinding{1};
    uvBinding.addAttribute(VertexAttribute::VertexAttributeType::UV0, VK_FORMAT_R32G32_SFLOAT);

    formatDescriptor.addBinding(defaultBinding);
    formatDescriptor.addBinding(uvBinding);

    setFormatDescriptor(formatDescriptor);
}

#include "SimpleMeshData.h"

SimpleMeshData::SimpleMeshData() {
    MeshDataFormatDescriptor formatDescriptor{};

    VertexBinding defaultBinding{0};
    defaultBinding.addAttribute(VertexAttribute::VertexAttributeType::Position, VK_FORMAT_R32G32B32_SFLOAT);
    defaultBinding.addAttribute(VertexAttribute::VertexAttributeType::Color, VK_FORMAT_R32G32B32_SFLOAT);
    defaultBinding.addAttribute(VertexAttribute::VertexAttributeType::Normal, VK_FORMAT_R32G32B32_SFLOAT);
    formatDescriptor.addBinding(defaultBinding);

    setFormatDescriptor(formatDescriptor);
}

void SimpleMeshData::setVertexCount(const size_t vertexCount) {
    MeshData::setVertexCount(vertexCount);

    vertices.resize(vertexCount);
}

void SimpleMeshData::setPositions(const std::vector<glm::vec3> &positions) {
    if (positions.size() != vertexCount)
        setVertexCount(positions.size());

    for (int i = 0; i < positions.size(); i++) {
        vertices[i].pos = positions[i];
    }
}

void SimpleMeshData::setNormals(const std::vector<glm::vec3> &normals) {
    if (normals.size() != vertexCount)
        setVertexCount(normals.size());

    for (int i = 0; i < normals.size(); i++) {
        vertices[i].normal = normals[i];
    }
}

void SimpleMeshData::setColors(const std::vector<glm::vec3> &colors) {
    if (colors.size() != vertexCount)
        setVertexCount(colors.size());

    for (int i = 0; i < colors.size(); i++) {
        vertices[i].color = colors[i];
    }
}

const void *SimpleMeshData::getRawVertexData() const {
    return vertices.data();
}

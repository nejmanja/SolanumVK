#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "MeshData.h"

class UVMappedMeshData : public MeshData {
public:
    UVMappedMeshData();

    [[nodiscard]] size_t getVertexSize() const override { return sizeof(Vertex); }

    [[nodiscard]] size_t getVertexCount() const override { return vertices.size(); }

    void setVertexCount(size_t vertexCount) override {
        MeshData::setVertexCount(vertexCount);
        vertices.resize(vertexCount);
    }

    [[nodiscard]] const void *getRawVertexData() const override { return vertices.data(); }

    void setPositions(const std::vector<glm::vec3> &positions) {
        if (positions.size() != vertexCount)
            setVertexCount(positions.size());

        for (int i = 0; i < positions.size(); i++) {
            vertices[i].pos = positions[i];
        }
    }

    void setNormals(const std::vector<glm::vec3> &normals) {
        if (normals.size() != vertexCount)
            setVertexCount(normals.size());

        for (int i = 0; i < normals.size(); i++) {
            vertices[i].normal = normals[i];
        }
    }

    void setUV0(const std::vector<glm::vec2> &uvs) {
        if (uvs.size() != vertexCount)
            setVertexCount(uvs.size());

        for (int i = 0; i < uvs.size(); i++) {
            vertices[i].uv0 = uvs[i];
        }
    }

private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv0;
    };

    std::vector<Vertex> vertices{};
};

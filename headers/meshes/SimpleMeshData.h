#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "MeshData.h"

class SimpleMeshData : public MeshData {
public:
    SimpleMeshData();

    void setVertexCount(size_t vertexCount) override;

    void setPositions(const std::vector<glm::vec3> &positions);

    void setNormals(const std::vector<glm::vec3> &normals);

    void setColors(const std::vector<glm::vec3> &colors);

    [[nodiscard]] size_t getVertexSize() const override { return sizeof(Vertex); }

    [[nodiscard]] const void *getRawVertexData() const override;

    [[nodiscard]] size_t getVertexCount() const override { return vertices.size(); }

private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec3 normal;
    };

    std::vector<Vertex> vertices;
};

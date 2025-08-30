#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "MeshData.h"

class SimpleMeshData : public MeshData
{
public:
    SimpleMeshData();

    void setVertexCount(size_t vertexCount) override;
    void setPositions(const std::vector<glm::vec3> &positions);
    void setColors(const std::vector<glm::vec3> &colors);
    const size_t getVertexSize() override { return sizeof(Vertex); }

    const void *getRawVertexData() const override;
    const size_t getVertexCount() const { return vertices.size(); }

private:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> vertices;
};
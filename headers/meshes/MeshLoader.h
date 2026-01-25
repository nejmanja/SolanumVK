#pragma once

#include <filesystem>
#include <fastgltf/core.hpp>

#include "SimpleMeshData.h"
#include "UVMappedMeshData.h"

class MeshLoader {
public:
    static SimpleMeshData loadSimpleMesh(const std::filesystem::path &path);

    static UVMappedMeshData loadUVMappedMesh(const std::filesystem::path &path);

    MeshLoader() = delete;

private:
    static std::vector<glm::vec3> readPositions(fastgltf::Expected<fastgltf::Asset> &asset,
                                                fastgltf::Primitive &primitive);

    static std::vector<glm::vec3> readNormals(fastgltf::Expected<fastgltf::Asset> &asset,
                                              fastgltf::Primitive &primitive);

    static std::vector<glm::vec3> readColors(fastgltf::Expected<fastgltf::Asset> &asset,
                                             fastgltf::Primitive &primitive);

    static std::vector<glm::vec2> readUVs(fastgltf::Expected<fastgltf::Asset> &asset,
                                          fastgltf::Primitive &primitive, const uint32_t channel);

    static std::vector<uint32_t> readIndices(fastgltf::Expected<fastgltf::Asset> &asset,
                                             fastgltf::Primitive &primitive);
};

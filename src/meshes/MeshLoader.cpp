#include "MeshLoader.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <glm/vec3.hpp>

#include <iostream>

#include "UVMappedMeshData.h"

std::vector<glm::vec3> MeshLoader::readPositions(fastgltf::Expected<fastgltf::Asset> &asset,
                                                 fastgltf::Primitive &primitive) {
    std::vector<glm::vec3> positions{};

    const auto &posAccessor = asset->accessors[primitive.findAttribute("POSITION")->accessorIndex];
    fastgltf::iterateAccessorWithIndex<glm::vec3>(
        asset.get(),
        posAccessor,
        [&](const glm::vec3 position, size_t index) {
            positions.emplace_back(position);
        });

    return positions;
}

std::vector<glm::vec3> MeshLoader::readNormals(fastgltf::Expected<fastgltf::Asset> &asset,
                                               fastgltf::Primitive &primitive) {
    std::vector<glm::vec3> normals{};

    const auto &normalAccessor = asset->accessors[primitive.findAttribute("NORMAL")->accessorIndex];
    fastgltf::iterateAccessorWithIndex<glm::vec3>(
        asset.get(),
        normalAccessor,
        [&](const glm::vec3 normal, size_t index) {
            normals.emplace_back(normal);
        });

    return normals;
}

std::vector<glm::vec3> MeshLoader::readColors(fastgltf::Expected<fastgltf::Asset> &asset,
                                              fastgltf::Primitive &primitive) {
    std::vector<glm::vec3> colors{};

    const auto &colAccessor = asset->accessors[primitive.findAttribute("COLOR_0")->accessorIndex];
    fastgltf::iterateAccessorWithIndex<glm::vec4>(
        asset.get(),
        colAccessor,
        [&](const glm::vec4 color, size_t index) {
            colors.emplace_back(color);
        });

    return colors;
}

std::vector<uint32_t> MeshLoader::readIndices(fastgltf::Expected<fastgltf::Asset> &asset,
                                              fastgltf::Primitive &primitive) {
    std::vector<uint32_t> indices{};

    auto &indexAccessor = asset->accessors[primitive.indicesAccessor.value()];
    fastgltf::iterateAccessor<uint32_t>(
        asset.get(),
        indexAccessor,
        [&](uint32_t idx) {
            indices.emplace_back(idx);
        });

    return indices;
}

std::vector<glm::vec2> MeshLoader::readUVs(fastgltf::Expected<fastgltf::Asset> &asset,
                                           fastgltf::Primitive &primitive, const uint32_t channel) {
    std::vector<glm::vec2> uvs{};

    std::string channelName = "UV_" + std::to_string(channel);

    auto &uvAccessor = asset->accessors[primitive.findAttribute(channelName)->accessorIndex];
    fastgltf::iterateAccessorWithIndex<glm::vec2>(
        asset.get(),
        uvAccessor,
        [&](const glm::vec2 uv, size_t index) {
            uvs.emplace_back(uv);
        });

    return uvs;
}

SimpleMeshData MeshLoader::loadSimpleMesh(const std::filesystem::path &path) {
    fastgltf::Parser parser;

    auto dataBuffer = fastgltf::GltfDataBuffer::FromPath(path);

    if (dataBuffer.error() != fastgltf::Error::None) {
        std::cout << "Failed to load mesh from path " << path << std::endl;
        abort();
    }

    auto asset = parser.loadGltf(dataBuffer.get(), path.parent_path());
    if (auto error = asset.error(); error != fastgltf::Error::None) {
        std::cout << "Failed to load mesh data from file " << path << std::endl;
        abort();
    }

    for (auto &&mesh: asset->meshes) {
        std::vector<uint32_t> indices{};
        std::vector<glm::vec3> positions{};
        std::vector<glm::vec3> colors{};
        std::vector<glm::vec3> normals{};

        SimpleMeshData meshData{};

        for (auto &&primitive: mesh.primitives) {
            // indices
            indices = readIndices(asset, primitive);
            // positions
            positions = readPositions(asset, primitive);
            // normals
            normals = readNormals(asset, primitive);
            // vertex colors
            colors = readColors(asset, primitive);
        }

        meshData.setIndices(indices);
        meshData.setVertexCount(positions.size());
        meshData.setPositions(positions);
        meshData.setColors(colors);
        meshData.setNormals(normals);

        return meshData;
    }

    return SimpleMeshData{};
}

UVMappedMeshData MeshLoader::loadUVMappedMesh(const std::filesystem::path &path) {
    fastgltf::Parser parser;

    auto dataBuffer = fastgltf::GltfDataBuffer::FromPath(path);

    if (dataBuffer.error() != fastgltf::Error::None) {
        std::cout << "Failed to load mesh from path " << path << std::endl;
        abort();
    }

    auto asset = parser.loadGltf(dataBuffer.get(), path.parent_path());
    if (auto error = asset.error(); error != fastgltf::Error::None) {
        std::cout << "Failed to load mesh data from file " << path << std::endl;
        abort();
    }

    for (auto &&mesh: asset->meshes) {
        std::vector<uint32_t> indices{};
        std::vector<glm::vec3> positions{};
        std::vector<glm::vec2> uv0{};
        std::vector<glm::vec3> normals{};

        UVMappedMeshData meshData{};

        for (auto &&primitive: mesh.primitives) {
            // indices
            indices = readIndices(asset, primitive);
            // positions
            positions = readPositions(asset, primitive);
            // normals
            normals = readNormals(asset, primitive);
            // uvs
            uv0 = readUVs(asset, primitive, 0);
        }

        meshData.setIndices(indices);
        meshData.setVertexCount(positions.size());
        meshData.setPositions(positions);
        meshData.setUV0(uv0);
        meshData.setNormals(normals);

        return meshData;
    }

    return UVMappedMeshData{};
}

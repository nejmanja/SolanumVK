#include "MeshLoader.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <glm/vec3.hpp>

#include <iostream>

SimpleMeshData MeshLoader::loadSimpleMesh(std::filesystem::path path)
{
    fastgltf::Parser parser;

    auto dataBuffer = fastgltf::GltfDataBuffer::FromPath(path);

    if (dataBuffer.error() != fastgltf::Error::None)
    {
        std::cout << "Failed to load mesh from path " << path << std::endl;
        abort();
    }

    auto asset = parser.loadGltf(dataBuffer.get(), path.parent_path());
    if (auto error = asset.error(); error != fastgltf::Error::None)
    {
        std::cout << "Failed to load mesh data from file " << path << std::endl;
        abort();
    }

    for (auto &&mesh : asset->meshes)
    {
        std::vector<uint32_t> indices{};
        std::vector<glm::vec3> positions{};
        std::vector<glm::vec3> colors{};
        std::vector<glm::vec3> normals{};

        SimpleMeshData meshData{};

        for (auto &&primitive : mesh.primitives)
        {
            // indices
            auto &indexAccessor = asset->accessors[primitive.indicesAccessor.value()];
            fastgltf::iterateAccessor<uint32_t>(
                asset.get(),
                indexAccessor,
                [&](uint32_t idx)
                {
                    indices.push_back(idx);
                });

            // positions
            auto &posAccessor = asset->accessors[primitive.findAttribute("POSITION")->accessorIndex];
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                asset.get(),
                posAccessor,
                [&](glm::vec3 position, size_t index)
                {
                    positions.push_back(position);
                });
            // positions
            auto &normalAccessor = asset->accessors[primitive.findAttribute("NORMAL")->accessorIndex];
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                asset.get(),
                normalAccessor,
                [&](glm::vec3 normal, size_t index)
                {
                    normals.push_back(normal);
                });
            // vertex colors
            auto &colAccessor = asset->accessors[primitive.findAttribute("COLOR_0")->accessorIndex];
            fastgltf::iterateAccessorWithIndex<glm::vec4>(
                asset.get(),
                colAccessor,
                [&](glm::vec4 color, size_t index)
                {
                    colors.push_back(color);
                });
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
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
        }

        meshData.setIndices(indices);
        meshData.setVertexCount(positions.size());
        meshData.setPositions(positions);

        return meshData;
    }

    return SimpleMeshData{};
}
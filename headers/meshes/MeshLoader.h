#pragma once

#include <filesystem>
#include <fastgltf/core.hpp>

#include "SimpleMeshData.h"

class MeshLoader
{
public:
    static SimpleMeshData loadSimpleMesh(std::filesystem::path path);

private:
    MeshLoader() = delete;
};
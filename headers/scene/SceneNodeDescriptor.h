#pragma once

#include <string>
#include <optional>
#include "GPUMesh.h"
#include "Transform.h"

struct SceneNodeDescriptor {
    std::string name{};
    Transform transform{};
    std::optional<GPUMesh> gpuMesh{std::nullopt};
};

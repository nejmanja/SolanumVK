#pragma once
#include <string>

#include "Transform.h"

struct SceneNodeDescriptor {
    std::string name{};
    Transform transform{};
};

#pragma once

#include <glm/glm.hpp>

struct SceneDescriptor {
    glm::mat4 view;
    glm::mat4 proj;

    glm::vec3 sunDirection;
    glm::vec3 sunColor;
};

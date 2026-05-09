#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>

void Transform::updateTransform() {
    const auto scaleMat = glm::scale(glm::mat4(1.0f), scale);
    const auto rotationMat = glm::mat4_cast(rotation);
    const auto translationMat = glm::translate(glm::mat4(1.0f), translation);
    transformMatrix = scaleMat * rotationMat * translationMat;

    dirty = false;
}

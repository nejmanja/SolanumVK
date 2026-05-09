#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform() = default;

    Transform(const Transform &other) {
        translation = other.translation;
        rotation = other.rotation;
        scale = other.scale;
        transformMatrix = other.transformMatrix;
        dirty = other.dirty;
    }

    Transform(const Transform &&other) noexcept {
        translation = other.translation;
        rotation = other.rotation;
        scale = other.scale;
        transformMatrix = other.transformMatrix;
        dirty = other.dirty;
    }

    Transform(const glm::vec3 translation, const glm::vec3 rotation, const glm::vec3 scale) {
        this->translation = translation;
        this->rotation = glm::quat(glm::radians(rotation));
        this->scale = scale;
        updateTransform();
    }

    explicit Transform(const glm::mat4 &matrix) {
        this->translation = matrix[3];
        this->scale = glm::vec3(matrix[0][0], matrix[1][1], matrix[2][2]);
        this->rotation = glm::quat_cast(matrix);
    }

    glm::mat4 getTransformMatrix() {
        if (dirty)
            updateTransform();

        return transformMatrix;
    }

    void setTranslation(const glm::vec3 &trans) {
        this->translation = trans;
        dirty = true;
    }

    void setRotation(const glm::quat rot) {
        this->rotation = rot;
        dirty = true;
    }

    void setScale(const glm::vec3 &s) {
        this->scale = s;
        dirty = true;
    }

    void setScale(const float s) {
        this->scale = glm::vec3(s);
        dirty = true;
    }

private:
    void updateTransform();

    glm::vec3 translation{};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
    // Identity
    glm::mat4 transformMatrix{1.0f};

    bool dirty{false};
};

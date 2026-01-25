#pragma once

#include <cmath>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera {
public:
    Camera() { updateViewMatrix(); }

    ~Camera() = default;

    void setPerspectiveProj(float fovY, float aspect, float zNear, float zFar);

    void setOrthoProj(float width, float aspect, float zNear, float zFar);

    void changeSpeed(const float delta) {
        camSpeed += delta;
        if (camSpeed < 0.5f)
            camSpeed = 0.5f;
    }

    void move(const glm::vec3 offset) {
        camPosition += offset.x * camSpeed * glm::cross(lookVector, glm::vec3{0.0f, 1.0f, 0.0f});
        camPosition += offset.y * camSpeed * glm::vec3{0.0f, 1.0f, 0.0f};
        camPosition += offset.z * camSpeed * lookVector;
        updateViewMatrix();
    }

    void rotate(const float yRot, const float zRot) {
        cameraRotY += yRot * MOUSE_SENSITIVITY;
        cameraRotZ -= zRot * MOUSE_SENSITIVITY;
        if (cameraRotZ > 89.0f)
            cameraRotZ = 89.0f;
        if (cameraRotZ < -89.0f)
            cameraRotZ = -89.0f;

        const auto cosTheta = std::cos(cameraRotY);
        const auto sinTheta = std::sin(cameraRotY);
        const auto cosPhi = std::cos(cameraRotZ);
        const auto sinPhi = std::sin(cameraRotZ);

        lookVector = glm::normalize(glm::vec3{cosPhi * cosTheta, sinPhi, cosPhi * sinTheta});
    }

    [[nodiscard]] const glm::mat4 &getView() const { return view; }
    [[nodiscard]] const glm::mat4 &getProj() const { return proj; }
    [[nodiscard]] const glm::vec3 &getLook() const { return lookVector; }

private:
    void updateViewMatrix();

    glm::mat4 view{1.0f};
    glm::mat4 proj{1.0f};
    glm::vec3 lookVector{0.0f, 0.0f, 1.0f};
    glm::vec3 camPosition{0.0f, 0.0f, -5.0f};

    float camSpeed{1.0f};
    float cameraRotY{1.57f}, cameraRotZ{0.0f};
    const float MOUSE_SENSITIVITY = 0.1f;
};

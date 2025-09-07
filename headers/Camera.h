#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera
{
public:
    Camera() { updateViewMatrix(); }
    ~Camera() {}

    void setPerspectiveProj(float fovY, float aspect, float zNear, float zFar);
    void setOrthoProj(float width, float aspect, float zNear, float zFar);
    void move(glm::vec3 offset)
    {
        camPosition += offset.x * glm::cross(lookVector, glm::vec3{0.0f, 1.0f, 0.0f});
        camPosition += offset.y * glm::vec3{0.0f, 1.0f, 0.0f};
        camPosition += offset.z * lookVector;
        updateViewMatrix();
    }

    const glm::mat4 &getView() const { return view; }
    const glm::mat4 &getProj() const { return proj; }

private:
    void updateViewMatrix();
    glm::mat4 view{1.0f};
    glm::mat4 proj{1.0f};
    glm::vec3 lookVector{0.0f, 0.0f, 1.0f};
    glm::vec3 camPosition{};
};
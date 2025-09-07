#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::setPerspectiveProj(float fovY, float aspect, float zNear, float zFar)
{
    proj = glm::perspective(fovY, aspect, zNear, zFar);
    proj[1][1] *= -1;
}

void Camera::setOrthoProj(float width, float aspect, float zNear, float zFar)
{
    proj = glm::ortho(-width, width, -aspect * width, aspect * width, zNear, zFar);
    proj[1][1] *= -1;
}

void Camera::updateViewMatrix()
{
    auto right = glm::cross(glm::vec3{0.0f, 1.0f, 0.0f}, lookVector);
    auto up = glm::cross(lookVector, right);

    view = glm::lookAt(camPosition, camPosition + lookVector, up);
}
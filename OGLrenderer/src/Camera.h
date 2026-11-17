#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::mat4 GetProjectionMatrix(float aspect) const
    {
        return glm::perspective(
            glm::radians(fov),
            aspect,
            nearPlane,
            farPlane
        );
    }
};
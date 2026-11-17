#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f }; // Euler angles, degrees
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

    glm::mat4 GetLocalMatrix() const
    {
        glm::mat4 model(1.0f);

        model = glm::translate(model, position);

        // Yaw, pitch, roll
        model = glm::rotate(model, glm::radians(rotation.y),
            glm::vec3(0, 1, 0));

        model = glm::rotate(model, glm::radians(rotation.x),
            glm::vec3(1, 0, 0));

        model = glm::rotate(model, glm::radians(rotation.z),
            glm::vec3(0, 0, 1));

        model = glm::scale(model, scale);

        return model;
    }
};
#pragma once

#include <string>
#include <optional>
#include <stdexcept>
#include <utility>

#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

class GameObject
{
private:
    GameObject* m_Parent = nullptr;

public:
    std::string name;

    Transform transform;

    // Optional components
    std::optional<Camera> camera;
    std::optional<MeshRenderer> meshRenderer;

    explicit GameObject(std::string name)
        : name(std::move(name))
    {
    }

    void SetParent(GameObject* parent)
    {
        // Prevent circular parenting
        for (GameObject* p = parent; p; p = p->m_Parent)
        {
            if (p == this)
                throw std::logic_error("Circular hierarchy");
        }

        m_Parent = parent;
    }

    GameObject* GetParent() const
    {
        return m_Parent;
    }

    glm::mat4 GetWorldMatrix() const
    {
        glm::mat4 local = transform.GetLocalMatrix();

        if (m_Parent)
            return m_Parent->GetWorldMatrix() * local;

        return local;
    }
};
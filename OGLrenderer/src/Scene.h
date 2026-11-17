#pragma once

#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

#include "GameObject.h"

class Scene
{
private:
    std::vector<std::unique_ptr<GameObject>> m_Objects;

    GameObject* m_ActiveCamera = nullptr;

public:
    GameObject& CreateObject(std::string name)
    {
        auto object =
            std::make_unique<GameObject>(std::move(name));

        GameObject& ref = *object;

        m_Objects.push_back(std::move(object));

        return ref;
    }

    void SetActiveCamera(GameObject& object)
    {
        if (!object.camera)
            throw std::logic_error(
                "GameObject does not have a camera"
            );

        m_ActiveCamera = &object;
    }

    GameObject* GetActiveCamera() const
    {
        return m_ActiveCamera;
    }

    const std::vector<std::unique_ptr<GameObject>>&
        GetObjects() const
    {
        return m_Objects;
    }
};
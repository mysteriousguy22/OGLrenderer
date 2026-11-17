#pragma once

#include <memory>

#include "Mesh.h"
#include "Material.h"

struct MeshRenderer
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};
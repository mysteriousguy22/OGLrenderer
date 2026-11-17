#pragma once

#include <memory>

#include "Shader.h"
#include "Texture.h"

struct Material
{
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
};
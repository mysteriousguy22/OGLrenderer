#pragma once

#include "Scene.h"
#include "Renderer.h"

void RenderScene(
    const Scene& scene,
    Renderer& renderer,
    int framebufferWidth,
    int framebufferHeight
);
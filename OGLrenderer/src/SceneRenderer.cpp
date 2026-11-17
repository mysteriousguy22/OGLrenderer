#include "SceneRenderer.h"

#include <glm/gtc/matrix_inverse.hpp>

void RenderScene(
    const Scene& scene,
    Renderer& renderer,
    int framebufferWidth,
    int framebufferHeight)
{
    if (framebufferWidth <= 0 || framebufferHeight <= 0)
        return;

    GameObject* cameraObject = scene.GetActiveCamera();

    if (!cameraObject || !cameraObject->camera)
        return;

    // 1. Camera matrices
    float aspect =
        static_cast<float>(framebufferWidth) /
        static_cast<float>(framebufferHeight);

    glm::mat4 view =
        glm::inverse(cameraObject->GetWorldMatrix());

    glm::mat4 projection =
        cameraObject->camera->GetProjectionMatrix(aspect);

    // 2. Draw each renderable GameObject
    for (const auto& object : scene.GetObjects())
    {
        if (!object->meshRenderer)
            continue;

        const MeshRenderer& meshRenderer =
            *object->meshRenderer;

        if (!meshRenderer.mesh || !meshRenderer.material)
            continue;

        Material& material = *meshRenderer.material;

        if (!material.shader)
            continue;

        Shader& shader = *material.shader;

        shader.Bind();

        // 3. Set model, view and projection
        shader.SetUniformMat4f(
            "u_Model",
            object->GetWorldMatrix()
        );

        shader.SetUniformMat4f("u_View", view);
        shader.SetUniformMat4f("u_Projection", projection);

        // 4. Bind material texture
        if (material.texture)
        {
            material.texture->Bind(0);
            shader.SetUniform1i("u_Texture", 0);
        }

        // 5. Draw geometry
        renderer.Draw(
            meshRenderer.mesh->GetVertexArray(),
            meshRenderer.mesh->GetIndexBuffer(),
            shader
        );
    }
}
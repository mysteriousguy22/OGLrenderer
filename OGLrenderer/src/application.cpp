
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Renderer.h"
#include "Scene.h"
#include "SceneRenderer.h"
#include "Mesh.h"
#include "Material.h"


void FramebufferSizeCallback(
    GLFWwindow* window, int width, int height)
{
    GLCall(glViewport(0, 0, width, height));
}


void MouseCallback(
    GLFWwindow* window, double xpos, double ypos)
{
    Scene* scene = static_cast<Scene*>(
        glfwGetWindowUserPointer(window)
        );

    if (!scene)
        return;

    GameObject* camera = scene->GetActiveCamera();

    if (!camera)
        return;

    static bool firstMouse = true;
    static double lastX = 0.0;
    static double lastY = 0.0;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos);

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;

    camera->transform.rotation.y -= xoffset * sensitivity;

    camera->transform.rotation.x = std::clamp(
        camera->transform.rotation.x + yoffset * sensitivity,
        -89.0f,
        89.0f
    );
}

void ProcessInput(
    GLFWwindow* window,
    Scene& scene,
    float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    GameObject* camera = scene.GetActiveCamera();

    if (!camera)
        return;

    glm::mat4 world = camera->GetWorldMatrix();

    // Camera's forward direction
    glm::vec3 forward = glm::normalize(
        glm::vec3(world * glm::vec4(0, 0, -1, 0))
    );

    // Ignore vertical camera rotation for movement
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    // Horizontal right direction
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(
        glm::cross(forward, up)
    );

    glm::vec3 movement(0.0f);

    // Horizontal movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement += forward;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement -= forward;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement -= right;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement += right;

    // Vertical movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        movement += up;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        movement -= up;

    // Normalize to prevent faster diagonal movement
    if (glm::length(movement) > 0.0f)
        movement = glm::normalize(movement);

    const float speed = 5.0f;

    camera->transform.position += movement * speed * deltaTime;
}


std::shared_ptr<Mesh> CreateCubeMesh()
{
    // Each face has 4 vertices:
    // x, y, z, u, v

    std::vector<Vertex> vertices = {

        // Front (+Z)
        {-0.5f, -0.5f,  0.5f, 0, 0},
        { 0.5f, -0.5f,  0.5f, 1, 0},
        { 0.5f,  0.5f,  0.5f, 1, 1},
        {-0.5f,  0.5f,  0.5f, 0, 1},

        // Back (-Z)
        { 0.5f, -0.5f, -0.5f, 0, 0},
        {-0.5f, -0.5f, -0.5f, 1, 0},
        {-0.5f,  0.5f, -0.5f, 1, 1},
        { 0.5f,  0.5f, -0.5f, 0, 1},

        // Left (-X)
        {-0.5f, -0.5f, -0.5f, 0, 0},
        {-0.5f, -0.5f,  0.5f, 1, 0},
        {-0.5f,  0.5f,  0.5f, 1, 1},
        {-0.5f,  0.5f, -0.5f, 0, 1},

        // Right (+X)
        { 0.5f, -0.5f,  0.5f, 0, 0},
        { 0.5f, -0.5f, -0.5f, 1, 0},
        { 0.5f,  0.5f, -0.5f, 1, 1},
        { 0.5f,  0.5f,  0.5f, 0, 1},

        // Top (+Y)
        {-0.5f,  0.5f,  0.5f, 0, 0},
        { 0.5f,  0.5f,  0.5f, 1, 0},
        { 0.5f,  0.5f, -0.5f, 1, 1},
        {-0.5f,  0.5f, -0.5f, 0, 1},

        // Bottom (-Y)
        {-0.5f, -0.5f, -0.5f, 0, 0},
        { 0.5f, -0.5f, -0.5f, 1, 0},
        { 0.5f, -0.5f,  0.5f, 1, 1},
        {-0.5f, -0.5f,  0.5f, 0, 1}
    };

    std::vector<unsigned int> indices;
    indices.reserve(36);

    for (unsigned int face = 0; face < 6; face++)
    {
        unsigned int i = face * 4;

        indices.insert(indices.end(), {
            i + 0, i + 1, i + 2,
            i + 2, i + 3, i + 0
        });
    }

    return std::make_shared<Mesh>(vertices, indices);
}


int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        800, 600, "OGLrenderer", nullptr, nullptr
    );

    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    GLClearErrors();

    glfwSetFramebufferSizeCallback(
        window, FramebufferSizeCallback
    );

    int initialWidth, initialHeight;
    glfwGetFramebufferSize(window,
        &initialWidth, &initialHeight);
    GLCall(glViewport(0, 0, initialWidth, initialHeight));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    {
        Scene scene;
        Renderer renderer;

        glfwSetWindowUserPointer(window, &scene);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetInputMode(window, GLFW_CURSOR,
            GLFW_CURSOR_DISABLED);


        auto shader = std::make_shared<Shader>(
            "res/shaders/vertex.glsl",
            "res/shaders/fragment.glsl"
        );

        auto texture = std::make_shared<Texture>(
            "res/textures/grass_block_side.png"
        );

        auto material = std::make_shared<Material>(
            Material{ shader, texture }
        );

        auto cubeMesh = CreateCubeMesh();

        GameObject& camera =
            scene.CreateObject("Main Camera");

        camera.transform.position = { 0.0f, 0.0f, 3.0f };
        camera.camera.emplace();

        scene.SetActiveCamera(camera);

        glm::vec3 cubePositions[] = {
            { 0.0f,  0.0f,   0.0f},
            { 2.0f,  5.0f, -15.0f},
            {-1.5f, -2.2f,  -2.5f},
            {-3.8f, -2.0f, -12.3f},
            { 2.4f, -0.4f,  -3.5f},
            {-1.7f,  3.0f,  -7.5f},
            { 1.3f, -2.0f,  -2.5f},
            { 1.5f,  2.0f,  -2.5f},
            { 1.5f,  0.2f,  -1.5f},
            {-1.3f,  1.0f,  -1.5f}
        };

        for (unsigned int i = 0; i < 10; i++)
        {
            GameObject& cube = scene.CreateObject(
                "Cube " + std::to_string(i)
            );

            cube.transform.position = cubePositions[i];
            cube.transform.rotation.x = 20.0f * i;

            cube.meshRenderer.emplace(
                MeshRenderer{ cubeMesh, material }
            );
        }

        GameObject& parent =
            scene.CreateObject("Parent Cube");

        parent.transform.position = { -2.0f, 0.0f, -5.0f };
        parent.meshRenderer.emplace(
            MeshRenderer{ cubeMesh, material }
        );

        GameObject& child =
            scene.CreateObject("Child Cube");

        child.SetParent(&parent);
        child.transform.position = { 2.0f, 0.0f, 0.0f };
        child.transform.scale = { 0.5f, 0.5f, 0.5f };

        child.meshRenderer.emplace(
            MeshRenderer{ cubeMesh, material }
        );


        float lastFrame = static_cast<float>(glfwGetTime());

        while (!glfwWindowShouldClose(window))
        {
            float currentFrame =
                static_cast<float>(glfwGetTime());

            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            ProcessInput(window, scene, deltaTime);

            // Rotate parent; child follows automatically
            parent.transform.rotation.y +=
                30.0f * deltaTime;

            renderer.Clear();

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            RenderScene(scene, renderer, width, height);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwSetWindowUserPointer(window, nullptr);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

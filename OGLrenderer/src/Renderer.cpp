#include "Renderer.h"

#include <iostream>

void GLClearErrors()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, const unsigned int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << ") " << function << " " << file << ":" << line << std::endl;

        return false;
    }

    return true;
}

void Renderer::Clear()
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(VertexArray& va, IndexBuffer& ib, Shader& shader) 
{
    va.Bind();
    ib.Bind();
    shader.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
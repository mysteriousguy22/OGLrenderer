#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

struct Vertex
{
    float x, y, z; // Position
    float u, v;    // Texture coordinates
};

class Mesh
{
private:
    VertexArray m_VAO;

    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;

public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        if (vertices.empty() || indices.empty())
            throw std::invalid_argument("Mesh cannot be empty");

        m_VAO.Bind();

        m_VBO = std::make_unique<VertexBuffer>(
                vertices.data(), 
                static_cast<unsigned int>(
                vertices.size() * sizeof(Vertex))
        );

        VertexBufferLayout layout;

        layout.Push<float>(3); // x, y, z
        layout.Push<float>(2); // u, v

        m_VAO.AddBuffer(*m_VBO, layout);

        m_IBO = std::make_unique<IndexBuffer>(
            indices.data(),
            static_cast<unsigned int>(indices.size())
        );
    }

    VertexArray& GetVertexArray()
    {
        return m_VAO;
    }

    IndexBuffer& GetIndexBuffer()
    {
        return *m_IBO;
    }
};
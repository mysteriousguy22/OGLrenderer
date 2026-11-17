#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Shader.h"
#include "Renderer.h";


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(vertexPath, fragmentPath);

    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const {
    GLCall(glUseProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    return ShaderProgramSource(vertexCode, fragmentCode);
}

unsigned int Shader::CreateShader(
    const std::string& vertexShader,
    const std::string& fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0)
    {
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));

    // Check if linking was successful
    int result;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));

    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));

        char* message = (char*)alloca(length * sizeof(char));

        GLCall(glGetProgramInfoLog(program, length, &length, message));

        std::cout << "Failed to link shader program!" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(vs));
        GLCall(glDeleteShader(fs));
        GLCall(glDeleteProgram(program));

        return 0;
    }

    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile shader!" << std::endl;
        std::cout << message << std::endl;

        return 0;
    }

    return id;
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    int location = GetUniformLocation(name);

    GLCall(glUniform1i(location, value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = GetUniformLocation(name);

    GLCall(glUniform4f(location, v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    int location = GetUniformLocation(name);

    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	
    if (location == -1) {
        std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
    }

    m_UniformLocationCache[name] = location;

    return location;
}
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

#include "Shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath)
{
    if (geometryPath == nullptr)
    {
        std::cout << vertexPath << " " << fragmentPath << std::endl;
    }
    else
    {
        std::cout << vertexPath << " " << geometryPath << " " << fragmentPath << std::endl;
    }
    std::string vertexCode;
    std::string geometryCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    if (geometryPath != nullptr)
    {
        gShaderFile.open(geometryPath);
    }

    std::stringstream vShaderStream, gShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    if (geometryPath != nullptr)
    {
        gShaderStream << gShaderFile.rdbuf();
    }

    vShaderFile.close();
    fShaderFile.close();
    if (geometryPath != nullptr)
    {
        gShaderFile.close();
    }

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    if (geometryPath != nullptr)
    {
        geometryCode = gShaderStream.str();
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();

    GLuint vertex, geometry, fragment;
    GLint success;

    // OpenGL 버전 출력
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;
    std::cout << "GLSL Version: " << glslVersion << std::endl;

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    if (geometryPath != nullptr)
    {
        // geometry Shader
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    if (geometryPath != nullptr)
    {
        glAttachShader(ID, geometry);
    }
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Program에 링크된 쉐이더는 필요없으므로 삭제
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
    {
        glDeleteShader(geometry);
    }
}

Shader::Shader(const char *computePath)
{
    std::string computeCode;
    std::ifstream cShaderFile;
    std::stringstream cShaderStream;

    cShaderFile.open(computePath);
    cShaderStream << cShaderFile.rdbuf();
    computeCode = cShaderStream.str();

    const char *cShaderCode = computeCode.c_str();

    GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(compute);
}

void Shader::use()
{
    glUseProgram(ID);
}

unsigned int Shader::getID() const
{
    return ID;
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n";
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n";
        }
    }
}
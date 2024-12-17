#pragma once

#include <string>
#include "utils/Angel.h"

class Shader
{
public:
    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);
    Shader(const char *computePath);

    void use();
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setUint32(const std::string &name, uint32_t value) const
    {
        glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, mat);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, mat);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, mat);
    }

    unsigned int getID() const;

private:
    unsigned int ID;
    void checkCompileErrors(unsigned int shader, std::string type);
};
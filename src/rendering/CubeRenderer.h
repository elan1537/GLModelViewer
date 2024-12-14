#pragma once
#include <memory>
#include <vector>
#include "utils/Angel.h"
#include "Shader.h"

class CubeRenderer
{
public:
    CubeRenderer();
    ~CubeRenderer();

    bool Init(const std::vector<vec3> &positions);
    void Draw(const mat4 model, const mat4 view, const mat4 proj);

private:
    std::vector<vec3> _positions;
    GLuint VAO, VBO;
    std::unique_ptr<Shader> shader;
};
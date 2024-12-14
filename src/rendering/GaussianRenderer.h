#pragma once
#include <memory>

#include "Shader.h"
#include "data_processing/SplatData.h"

class GaussianRenderer
{
public:
    GaussianRenderer();
    ~GaussianRenderer();

    bool Init(const std::vector<GSPoint> &points);
    void Draw(const mat4 &model, const mat4 &view, const mat4 &proj);

private:
    GLuint VAO, VBO;
    size_t numPoints;
    std::unique_ptr<Shader> shader;
};
#pragma once
#include <memory>

#include "Shader.h"
#include "data_processing/SplatData.h"

class GaussianRenderer
{
    static const int SH_SIZE = 48;

public:
    GaussianRenderer();
    ~GaussianRenderer();

    bool Init(const std::vector<GSPoint> &points);
    void Draw(const mat4 &pose, const mat4 &projection, const vec4 &viewport, const vec2 &nearFar);

private:
    GLuint VAO, VBO, tboBuffer, tboTexture;
    size_t numPoints;
    std::unique_ptr<Shader> shader;
};
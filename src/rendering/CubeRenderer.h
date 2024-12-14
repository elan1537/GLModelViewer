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
    void GenerateSphere(std::vector<vec3> &sphereVerts, int stacks, int slices, float radius);

    std::vector<vec3> _positions;
    std::vector<vec3> _lines;

    GLuint VAO, VBO;             // For line rendering
    GLuint sphereVAO, sphereVBO; // For sphere rendering

    std::unique_ptr<Shader> shader;
    std::vector<vec3> _sphereVertices;
    GLsizei _sphereVertCount = 0; // Number of vertices for the sphere
};
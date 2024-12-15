#pragma once
#include <memory>
#include <vector>
#include "utils/Angel.h"
#include "Shader.h"

class CubeRenderer
{
    const vec3 SELECTED_COLOR = vec3(1.0f, 0.0f, 0.0f);
    const vec3 LINE_COLOR = vec3(0.0f, 1.0f, 0.0f);
    const vec3 UNSELECTED_COLOR = vec3(0.0f, 0.0f, 1.0f);

public:
    CubeRenderer();
    ~CubeRenderer();

    bool Init(const std::vector<mat4> &poses);
    void Draw(const mat4 model, const mat4 view, const mat4 proj);
    void SetSelectedIndex(int idx) { selectedIndex = idx; }

private:
    void GenerateSphere(std::vector<vec3> &sphereVerts, int stacks, int slices, float radius);
    void GenerateGrid();

    std::vector<mat4> _poses;

    GLuint VAO, VBO;             // For line rendering (positions)
    GLuint sphereVAO, sphereVBO; // For sphere rendering
    GLuint gridVAO, gridVBO, gridColorVBO;

    std::unique_ptr<Shader> shader;     // For sphere & line rendering (uniform color)
    std::unique_ptr<Shader> gridShader; // For grid rendering (attribute color)

    std::vector<vec3> _sphereVertices;
    GLsizei _sphereVertCount = 0; // Number of vertices for the sphere

    std::vector<vec3> _gridVertices;
    std::vector<vec3> _gridColors;

    // 라인용 데이터 (poses 사이 선)
    std::vector<vec3> _lines;

    int selectedIndex = -1; // 선택된 포즈 인덱스
};
#include <glad/glad.h>

#include "CubeRenderer.h"
#include <iostream>

CubeRenderer::CubeRenderer()
{
    // 생성 시 VAO/VBO 생성
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);

    shader = std::make_unique<Shader>("shaders/cube.vert", "shaders/cube.frag");
    std::cout << "Shader ID: " << shader->getID() << std::endl;
}

CubeRenderer::~CubeRenderer()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);
}

bool CubeRenderer::Init(const std::vector<vec3> &positions)
{
    _positions = positions;
    _lines.clear();

    // positions 사이를 선분으로 이음
    for (size_t i = 0; i + 1 < _positions.size(); ++i)
    {
        _lines.push_back(_positions[i]);
        _lines.push_back(_positions[i + 1]);
    }

    std::cout << "lines size: " << _lines.size() << std::endl;
    for (unsigned int i = 0; i < _lines.size(); i++)
    {
        std::cout << "line " << i << ": " << _lines[i] << std::endl;
    }

    // 라인용 VAO/VBO 설정
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * _lines.size(), _lines.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
    glBindVertexArray(0);

    // 구 지오메트리 생성
    // stacks=16, slices=16 정도로 설정 (적절히 변경 가능)
    GenerateSphere(_sphereVertices, 16, 16, 1.0f);
    _sphereVertCount = static_cast<GLsizei>(_sphereVertices.size());

    // 구용 VAO/VBO 설정
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, _sphereVertices.size() * sizeof(vec3), _sphereVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
    glBindVertexArray(0);

    return true;
}

void CubeRenderer::Draw(const mat4 model, const mat4 view, const mat4 proj)
{
    shader->use();

    shader->setMat4("u_view", view);
    shader->setMat4("u_proj", proj);

    // 먼저 라인 렌더링
    shader->setVec3("u_color", vec3(1.0f, 0.0f, 0.0f)); // 라인 색상: 빨강
    shader->setMat4("u_model", mat4(1.0f));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(_lines.size()));

    // 각 위치마다 구 렌더링
    shader->setVec3("u_color", vec3(0.0f, 0.0f, 1.0f)); // 구 색상: 파랑
    glBindVertexArray(sphereVAO);
    for (auto &pos : _positions)
    {
        // 위치 pos에 조그마한 구 렌더링
        mat4 sphereModel = Translate(pos) * Scale(0.05f, 0.05f, 0.05f);
        shader->setMat4("u_model", sphereModel);
        glDrawArrays(GL_TRIANGLES, 0, _sphereVertCount);
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "CubeRenderer::Draw OpenGL error: " << err << std::endl;
    }
}

void CubeRenderer::GenerateSphere(std::vector<vec3> &sphereVerts, int stacks, int slices, float radius)
{
    // stacks: 위도 분할 횟수
    // slices: 경도 분할 횟수
    // 각도 계산
    // 위도(theta): -π/2 ~ π/2
    // 경도(phi): -π ~ π
    // 삼각형 strip을 만들기 위해 각 세그먼트마다 두 개의 위도줄 사이를 연결

    sphereVerts.clear();
    for (int i = 0; i < stacks; i++)
    {
        float lat0 = M_PI * (-0.5f + (float)i / stacks);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        for (int j = 0; j < slices; j++)
        {
            float lng = 2.0f * M_PI * (float)(j) / slices;
            float x = cos(lng);
            float y = sin(lng);

            float lng_next = 2.0f * M_PI * (float)(j + 1) / slices;
            float x_next = cos(lng_next);
            float y_next = sin(lng_next);

            // 4점 정의
            vec3 v0(x * zr0 * radius, y * zr0 * radius, z0 * radius);
            vec3 v1(x * zr1 * radius, y * zr1 * radius, z1 * radius);
            vec3 v2(x_next * zr1 * radius, y_next * zr1 * radius, z1 * radius);
            vec3 v3(x_next * zr0 * radius, y_next * zr0 * radius, z0 * radius);

            // 두 삼각형 (v0, v1, v2), (v0, v2, v3)
            sphereVerts.push_back(v0);
            sphereVerts.push_back(v1);
            sphereVerts.push_back(v2);

            sphereVerts.push_back(v0);
            sphereVerts.push_back(v2);
            sphereVerts.push_back(v3);
        }
    }
}
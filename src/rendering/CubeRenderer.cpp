#include <glad/glad.h>

#include "CubeRenderer.h"
#include <iostream>

CubeRenderer::CubeRenderer()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glGenBuffers(1, &gridColorVBO);

    // sphere/line용 셰이더 (uniform color)
    shader = std::make_unique<Shader>("shaders/cube_uniform_color.vert", "shaders/cube.frag");
    // grid용 셰이더 (attribute color)
    gridShader = std::make_unique<Shader>("shaders/cube.vert", "shaders/cube.frag");

    // cube_uniform_color.vert는 u_color를 uniform으로 받는 버전
    std::cout << "Shader ID: " << shader->getID() << ", GridShader ID: " << gridShader->getID() << std::endl;
}

CubeRenderer::~CubeRenderer()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);

    glDeleteBuffers(1, &gridVBO);
    glDeleteBuffers(1, &gridColorVBO);
    glDeleteVertexArrays(1, &gridVAO);
}

bool CubeRenderer::Init(const std::vector<mat4> &poses)
{
    _poses = poses;

    // 포즈들 사이 선을 생성
    _lines.clear();
    for (size_t i = 0; i + 1 < _poses.size(); ++i)
    {
        vec3 pos0(_poses[i][0][3], _poses[i][1][3], _poses[i][2][3]);
        vec3 pos1(_poses[i + 1][0][3], _poses[i + 1][1][3], _poses[i + 1][2][3]);
        _lines.push_back(pos0);
        _lines.push_back(pos1);
    }

    // 라인 VBO 설정
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * _lines.size(), _lines.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // 위치
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
    glBindVertexArray(0);

    // 그리드 생성
    GenerateGrid();
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * _gridVertices.size(), _gridVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, gridColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * _gridColors.size(), _gridColors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // 색상 attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);

    glBindVertexArray(0);

    // 구 지오메트리 생성
    GenerateSphere(_sphereVertices, 16, 16, 1.0f);
    _sphereVertCount = (GLsizei)_sphereVertices.size();

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
    // 1. Grid 렌더링 (attribute color 사용)
    gridShader->use();
    gridShader->setMat4("u_view", view);
    gridShader->setMat4("u_proj", proj);

    glBindVertexArray(gridVAO);

    for (auto &pose : _poses)
    {
        mat4 gridModel = pose * Scale(0.05f, 0.05f, 0.05f);
        gridShader->setMat4("u_model", gridModel);
        glDrawArrays(GL_LINES, 0, (GLsizei)_gridVertices.size());
    }

    // 2. Lines 렌더링 (uniform color 사용)
    // sphere/line용 셰이더 사용

    shader->use();
    shader->setMat4("u_view", view);
    shader->setMat4("u_proj", proj);

    // line은 uniform color 사용 (예: 초록색)
    shader->setVec3("u_color", LINE_COLOR);
    shader->setMat4("u_model", mat4(1.0f));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, (GLsizei)_lines.size());

    // 3. Sphere 렌더링 (uniform color 사용)

    glBindVertexArray(sphereVAO);
    for (size_t i = 0; i < _poses.size(); i++)
    {
        const mat4 &pose = _poses[i];
        vec3 pos(pose[0][3], pose[1][3], pose[2][3]);
        mat4 sphereModel = Translate(pos);

        // selectIndex인 경우 SELECTED_COLOR, 아니면 UNSELECTED_COLOR
        if ((int)i == selectedIndex)
        {
            sphereModel = sphereModel * Scale(0.01f, 0.01f, 0.01f);
            shader->setVec3("u_color", SELECTED_COLOR);
        }
        else
        {
            sphereModel = sphereModel * Scale(0.01f, 0.01f, 0.01f);
            shader->setVec3("u_color", UNSELECTED_COLOR);
        }

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

void CubeRenderer::GenerateGrid()
{
    _gridVertices.clear();
    _gridColors.clear();

    // x축
    _gridVertices.push_back(vec3(0.0f, 0.0f, 0.0f));
    _gridVertices.push_back(vec3(1.0f, 0.0f, 0.0f));
    _gridColors.push_back(vec3(1.0f, 0.0f, 0.0f));
    _gridColors.push_back(vec3(1.0f, 0.0f, 0.0f));

    // y축
    _gridVertices.push_back(vec3(0.0f, 0.0f, 0.0f));
    _gridVertices.push_back(vec3(0.0f, 1.0f, 0.0f));
    _gridColors.push_back(vec3(0.0f, 1.0f, 0.0f));
    _gridColors.push_back(vec3(0.0f, 1.0f, 0.0f));

    // z축
    _gridVertices.push_back(vec3(0.0f, 0.0f, 0.0f));
    _gridVertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    _gridColors.push_back(vec3(0.0f, 0.0f, 1.0f));
    _gridColors.push_back(vec3(0.0f, 0.0f, 1.0f));
}
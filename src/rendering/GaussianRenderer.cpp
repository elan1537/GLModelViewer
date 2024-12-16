#include <glad/glad.h>

#include "GaussianRenderer.h"

GaussianRenderer::GaussianRenderer()
{
    std::cout << "GaussianRenderer::GaussianRenderer" << std::endl;
    shader = std::make_unique<Shader>("shaders/gaussian.vert", "shaders/gaussian.frag", "shaders/gaussian.geo");
    shader->use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // TBO 관련 버퍼와 텍스처 생성용 멤버 변수
    tboBuffer = 0;
    tboTexture = 0;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout << "Shader ID: " << shader->getID() << std::endl;
}

GaussianRenderer::~GaussianRenderer()
{
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);

    // TBO 정리
    if (tboTexture != 0)
        glDeleteTextures(1, &tboTexture);
    if (tboBuffer != 0)
        glDeleteBuffers(1, &tboBuffer);
}

bool GaussianRenderer::Init(const std::vector<GSPoint> &points)
{
    std::cout << "GaussianRenderer::Init with " << points.size() << " points" << std::endl;
    numPoints = points.size();

    if (numPoints == 0)
    {
        std::cerr << "No points to render" << std::endl;
        return false;
    }

    glBindVertexArray(VAO);

    // 정점 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GSPoint) * points.size(), points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, opacity));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, scale));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, rotation));

    // SH 데이터만 모아서 TBO에 넣기
    std::vector<float> allSHData;
    allSHData.reserve(points.size() * SH_SIZE);

    for (size_t i = 0; i < points.size(); i++)
    {
        for (int j = 0; j < SH_SIZE; j++)
        {
            allSHData.push_back(points[i].shs.shs[j]);
        }
    }

    // Texture Buffer Object 생성
    glGenBuffers(1, &tboBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER, tboBuffer);
    glBufferData(GL_TEXTURE_BUFFER, allSHData.size() * sizeof(float), allSHData.data(), GL_STATIC_DRAW);

    glGenTextures(1, &tboTexture);
    glBindTexture(GL_TEXTURE_BUFFER, tboTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboBuffer);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glBindVertexArray(0);

    std::vector<uint32_t> keys(numPoints, 0);
    std::vector<uint32_t> vals(numPoints);

    for (uint32_t i = 0; i < numPoints; i++)
        vals[i] = i;

    GLuint keyBuffer;
    glGenBuffers(1, &keyBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, keyBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, keys.size() * sizeof(uint32_t), keys.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, keyBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GLuint valBuffer;
    glGenBuffers(1, &valBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, valBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, vals.size() * sizeof(uint32_t), vals.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, valBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return true;
}

void GaussianRenderer::Draw(const mat4 &pose, const mat4 &projection, const vec4 &viewport, const vec2 &nearFar)
{
    shader->use();

    float width = viewport.z;
    float height = viewport.w;
    float aspectRatio = width / height;

    vec3 eye = vec3(pose[3].x, pose[3].y, pose[3].z);

    shader->setMat4("viewMat", const_cast<mat4 &>(pose));
    shader->setMat4("projMat", const_cast<mat4 &>(projection));
    shader->setVec4("projParam", vec4(0.0, nearFar.x, nearFar.y, 0.0));
    shader->setVec4("viewport", const_cast<vec4 &>(viewport));
    shader->setVec3("eye", const_cast<vec3 &>(eye));

    // u_shBuffer를 0번 텍스처 유닛에 바인딩
    GLint shBufferLocation = glGetUniformLocation(shader->getID(), "u_shBuffer");
    glUniform1i(shBufferLocation, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, tboTexture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL error: " << err << std::endl;
    }
}
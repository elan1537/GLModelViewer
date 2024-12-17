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

    void generateSSBO(GLenum target, GLuint &obj, const void *data, size_t size, GLenum flag);

    bool Init(const std::vector<GSPoint> &points);
    void Draw(const mat4 &pose, const mat4 &projection, const vec4 &viewport, const vec2 &nearFar);
    void Sort(const mat4 &cameraMat, const mat4 &projMat, const vec2 &nearFar);
    void ReadSSBO(GLenum target, GLuint obj, uint32_t bufferSize, void *data);

    uint32_t numBlocksPerWorkgroup = 1024;

private:
    void BindVAO(const std::vector<GSPoint> &points);
    // 정점 및 셰이더 관련 변수
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> presortShader;
    std::unique_ptr<Shader> radixSortShader;
    std::unique_ptr<Shader> radixSortHistogramShader;

    uint32_t numPoints = 0;

    // OpenGL 객체들
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint indexEBO = 0;
    GLuint shSSBO = 0;

    GLuint posBufferObj = 0;
    GLuint keyBufferObj = 0;
    GLuint keyBuffer2Obj = 0;
    GLuint valBufferObj = 0;
    GLuint valBuffer2Obj = 0;
    GLuint histogramBufferObj = 0;
    GLuint atomicCounterBufferObj = 0;
    GLuint lengthBufferObj = 0;
    uint32_t sortCount;

    // 정렬 및 SH 데이터 관련 벡터
    std::vector<vec4> posVec;
    std::vector<uint32_t> indexVec;
    std::vector<uint32_t> depthVec;
    std::vector<uint32_t> atomicCounterVec = {0};
};
#include <glad/glad.h>

#include "GaussianRenderer.h"
#include <cstring>
#include <cassert>

void GL_ERROR_CHECK()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL error: " << err << std::endl;
    }
}

mat4 inverse(const mat4 &mat)
{
    GLfloat inv[16], det;
    mat4 out;

    const GLfloat *m = reinterpret_cast<const GLfloat *>(&mat); // 첫 원소의 주소 (row-major)
    // 이제 m[i]로 0~15 인덱스로 접근 가능 (m[0..3]: 첫 행, m[4..7]: 두 번째 행, ...)

    inv[0] = m[5] * m[10] * m[15] -
             m[5] * m[11] * m[14] -
             m[9] * m[6] * m[15] +
             m[9] * m[7] * m[14] +
             m[13] * m[6] * m[11] -
             m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] +
             m[4] * m[11] * m[14] +
             m[8] * m[6] * m[15] -
             m[8] * m[7] * m[14] -
             m[12] * m[6] * m[11] +
             m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] -
             m[4] * m[11] * m[13] -
             m[8] * m[5] * m[15] +
             m[8] * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] +
              m[4] * m[10] * m[13] +
              m[8] * m[5] * m[14] -
              m[8] * m[6] * m[13] -
              m[12] * m[5] * m[10] +
              m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] +
             m[1] * m[11] * m[14] +
             m[9] * m[2] * m[15] -
             m[9] * m[3] * m[14] -
             m[13] * m[2] * m[11] +
             m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] -
             m[0] * m[11] * m[14] -
             m[8] * m[2] * m[15] +
             m[8] * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] +
             m[0] * m[11] * m[13] +
             m[8] * m[1] * m[15] -
             m[8] * m[3] * m[13] -
             m[12] * m[1] * m[11] +
             m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] -
              m[0] * m[10] * m[13] -
              m[8] * m[1] * m[14] +
              m[8] * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] -
             m[1] * m[7] * m[14] -
             m[5] * m[2] * m[15] +
             m[5] * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] +
             m[0] * m[7] * m[14] +
             m[4] * m[2] * m[15] -
             m[4] * m[3] * m[14] -
             m[12] * m[2] * m[7] +
             m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] -
              m[0] * m[7] * m[13] -
              m[4] * m[1] * m[15] +
              m[4] * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] +
              m[0] * m[6] * m[13] +
              m[4] * m[1] * m[14] -
              m[4] * m[2] * m[13] -
              m[12] * m[1] * m[6] +
              m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
             m[1] * m[7] * m[10] +
             m[5] * m[2] * m[11] -
             m[5] * m[3] * m[10] -
             m[9] * m[2] * m[7] +
             m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
              m[0] * m[7] * m[9] +
              m[4] * m[1] * m[11] -
              m[4] * m[3] * m[9] -
              m[8] * m[1] * m[7] +
              m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (det == 0.0f)
        throw std::runtime_error("Matrix is not invertible!");

    det = 1.0f / det;

    for (int i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

    // inv[]를 mat4 out에 복사
    // mat[row][col] = inv[row*4+col]
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            out[row][col] = inv[row * 4 + col];
        }
    }

    return out;
}

GaussianRenderer::GaussianRenderer()
{
    std::cout << "GaussianRenderer::GaussianRenderer" << std::endl;
    shader = std::make_unique<Shader>("shaders/gaussian.vert", "shaders/gaussian.frag", "shaders/gaussian.geo");
    shader->use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout << "Shader ID: " << shader->getID() << std::endl;

    presortShader = std::make_unique<Shader>("shaders/presort.comp");
    std::cout << "Presort Shader ID: " << presortShader->getID() << std::endl;

    radixSortShader = std::make_unique<Shader>("shaders/multi_radixsort.comp");
    std::cout << "Radix Sort Shader ID: " << radixSortShader->getID() << std::endl;

    radixSortHistogramShader = std::make_unique<Shader>("shaders/multi_radixsort_histogram.comp");
    std::cout << "Radix Sort Histogram Shader ID: " << radixSortHistogramShader->getID() << std::endl;
}

GaussianRenderer::~GaussianRenderer()
{
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

void GaussianRenderer::BindVAO(const std::vector<GSPoint> &points)
{
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

    std::vector<float> shData;
    for (const auto &point : points)
        for (int i = 0; i < SH_SIZE; i++)
            shData.push_back(point.shs.shs[i]);

    generateSSBO(GL_SHADER_STORAGE_BUFFER, shSSBO, shData.data(), sizeof(float) * shData.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, shSSBO);

    generateSSBO(GL_ELEMENT_ARRAY_BUFFER, indexEBO, indexVec.data(), sizeof(uint32_t) * indexVec.size(), GL_DYNAMIC_STORAGE_BIT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexEBO);
    glBindVertexArray(0);
}

void GaussianRenderer::generateSSBO(GLenum target, GLuint &obj, const void *data, size_t size, GLenum flag)
{
    glGenBuffers(1, &obj);
    glBindBuffer(target, obj);
    glBufferStorage(target, size, data, flag);
    glBindBuffer(target, 0);
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

    // posVec 생성: points의 position을 vec4( x, y, z, 1.0 ) 형태로 저장
    posVec.clear();
    posVec.reserve(numPoints);
    for (const auto &point : points)
    {
        posVec.emplace_back(point.position.x, point.position.y, point.position.z, 1.0f);
    }

    depthVec.clear();
    depthVec.resize(numPoints);

    indexVec.reserve(points.size());
    assert(points.size() <= std::numeric_limits<uint32_t>::max());
    for (uint32_t i = 0; i < (uint32_t)points.size(); i++)
    {
        indexVec.push_back(i);
    }

    std::cout << "depthVec: " << depthVec.size() << std::endl;
    std::cout << "indexVec: " << indexVec.size() << std::endl;
    std::cout << posVec[10].x << ", " << posVec[10].y << ", " << posVec[10].z << std::endl;

    BindVAO(points);

    generateSSBO(GL_SHADER_STORAGE_BUFFER, posBufferObj, posVec.data(), sizeof(vec4) * posVec.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

    generateSSBO(GL_SHADER_STORAGE_BUFFER, keyBufferObj, depthVec.data(), sizeof(uint32_t) * depthVec.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
    generateSSBO(GL_SHADER_STORAGE_BUFFER, keyBuffer2Obj, depthVec.data(), sizeof(uint32_t) * depthVec.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

    const uint32_t NUM_ELEMENTS = static_cast<uint32_t>(numPoints);
    const uint32_t NUM_WORKGROUPS = (NUM_ELEMENTS + numBlocksPerWorkgroup - 1) / numBlocksPerWorkgroup;
    const uint32_t RADIX_SORT_BINS = 256;

    std::vector<uint32_t> histogramVec(NUM_WORKGROUPS * RADIX_SORT_BINS, 0);
    generateSSBO(GL_SHADER_STORAGE_BUFFER, histogramBufferObj, histogramVec.data(), sizeof(uint32_t) * histogramVec.size(), GL_DYNAMIC_STORAGE_BIT);

    generateSSBO(GL_SHADER_STORAGE_BUFFER, valBufferObj, indexVec.data(), sizeof(uint32_t) * indexVec.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
    generateSSBO(GL_SHADER_STORAGE_BUFFER, valBuffer2Obj, indexVec.data(), sizeof(uint32_t) * indexVec.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

    generateSSBO(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBufferObj, atomicCounterVec.data(), sizeof(uint32_t) * atomicCounterVec.size(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

    glBindVertexArray(0);

    return true;
}

void GaussianRenderer::Draw(const mat4 &pose, const mat4 &projection, const vec4 &viewport, const vec2 &nearFar)
{
    shader->use();

    float width = viewport.z;
    float height = viewport.w;
    float aspectRatio = width / height;

    vec3 eye = vec3(pose[3].x, pose[3].y, pose[3].z);

    mat4 viewMat = inverse(pose);

    shader->setMat4("viewMat", const_cast<mat4 &>(viewMat));
    shader->setMat4("projMat", const_cast<mat4 &>(projection));
    shader->setVec4("projParam", vec4(0.0, nearFar.x, nearFar.y, 0.0));
    shader->setVec4("viewport", const_cast<vec4 &>(viewport));
    shader->setVec3("eye", const_cast<vec3 &>(eye));

    glBindVertexArray(VAO);
    glDrawElements(GL_POINTS, sortCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GaussianRenderer::ReadSSBO(GLenum target, GLuint obj, uint32_t bufferSize, void *data)
{
    glBindBuffer(target, obj);
    void *rawBuffer = glMapBufferRange(target, 0, bufferSize, GL_MAP_READ_BIT);
    if (rawBuffer)
    {
        memcpy(data, rawBuffer, bufferSize);
    }
    glUnmapBuffer(target);
    glBindBuffer(target, 0);
}

void GaussianRenderer::Sort(const mat4 &cameraMat, const mat4 &projMat, const vec2 &nearFar)
{
    std::cout << "GaussianRenderer::Sort() begin" << std::endl;
    const uint32_t NUM_BYTES = 4;
    const uint32_t MAX_DEPTH = std::numeric_limits<uint32_t>::max();
    const uint32_t LOCAL_SIZE = 256;

    mat4 MVP = projMat * inverse(cameraMat);

    // Presort Block
    presortShader->use();
    presortShader->setMat4("modelViewProj", MVP);
    presortShader->setVec2("nearFar", nearFar);
    presortShader->setUint32("keyMax", MAX_DEPTH);

    atomicCounterVec[0] = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBufferObj);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(uint32_t), atomicCounterVec.data());
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBufferObj);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, keyBufferObj);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valBufferObj);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 4, atomicCounterBufferObj);

    glDispatchCompute(((GLuint)numPoints + (LOCAL_SIZE - 1)) / LOCAL_SIZE, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT); // 메모리 배리어로 GPU 연산 완료 대기

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBufferObj); // atomicCounterBufferObj에 count 기록

    // atomicCounterVec 읽기
    ReadSSBO(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBufferObj, sizeof(uint32_t) * atomicCounterVec.size(), atomicCounterVec.data());

    sortCount = atomicCounterVec[0];
    std::cout << "sortCount: " << sortCount << std::endl;

    assert(sortCount <= (uint32_t)numPoints);

    // RadixSort start
    const uint32_t NUM_ELEMENTS = (uint32_t)sortCount;
    const uint32_t NUM_WORKGROUPS = (NUM_ELEMENTS + numBlocksPerWorkgroup - 1) / numBlocksPerWorkgroup;

    // RadixSort
    radixSortShader->use();
    radixSortShader->setUint32("g_num_elements", NUM_ELEMENTS);
    radixSortShader->setUint32("g_num_workgroups", NUM_WORKGROUPS);
    radixSortShader->setUint32("g_num_blocks_per_workgroup", numBlocksPerWorkgroup);

    radixSortHistogramShader->use();
    radixSortHistogramShader->setUint32("g_num_elements", NUM_ELEMENTS);
    radixSortHistogramShader->setUint32("g_num_workgroups", NUM_WORKGROUPS);
    radixSortHistogramShader->setUint32("g_num_blocks_per_workgroup", numBlocksPerWorkgroup);

    for (uint32_t i = 0; i < NUM_BYTES; i++)
    {
        radixSortHistogramShader->use();
        radixSortHistogramShader->setUint32("g_shift", 8 * i);

        if (i == 0 || i == 2)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, keyBufferObj);
        else
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, keyBuffer2Obj);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, histogramBufferObj);
        glDispatchCompute(NUM_WORKGROUPS, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        radixSortShader->use();
        radixSortShader->setUint32("g_shift", 8 * i);

        if ((i % 2) == 0)
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, keyBufferObj);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, keyBuffer2Obj);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valBufferObj);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, valBuffer2Obj);
        }
        else
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, keyBuffer2Obj);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, keyBufferObj);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valBuffer2Obj);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, valBufferObj);
        }

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, histogramBufferObj);
        glDispatchCompute(NUM_WORKGROUPS, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    GL_ERROR_CHECK();

    std::vector<uint32_t> sortedKeyVec(sortCount, 0);
    ReadSSBO(GL_SHADER_STORAGE_BUFFER, keyBufferObj, sizeof(uint32_t) * sortedKeyVec.size(), sortedKeyVec.data());

    if (NUM_BYTES % 2 == 1)
    {
        glBindBuffer(GL_COPY_READ_BUFFER, valBuffer2Obj);
    }
    else
    {
        glBindBuffer(GL_COPY_READ_BUFFER, valBufferObj);
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, indexEBO);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sortCount * sizeof(uint32_t));
    std::cout << "glCopyBufferSubData done" << std::endl;
    GL_ERROR_CHECK();
}

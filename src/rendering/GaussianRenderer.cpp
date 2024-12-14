#include <glad/glad.h>

#include "GaussianRenderer.h"

GaussianRenderer::GaussianRenderer()
{
    std::cout << "GaussianRenderer::GaussianRenderer" << std::endl;
    shader = std::make_unique<Shader>("shaders/gaussian.vert", "shaders/gaussian.frag");
    shader->use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GSPoint) * points.size(), points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GSPoint), (void *)offsetof(GSPoint, color));

    return true;
}

void GaussianRenderer::Draw(const mat4 &model, const mat4 &view, const mat4 &proj)
{
    shader->use();

    shader->setMat4("u_model", const_cast<mat4 &>(model));
    shader->setMat4("u_view", const_cast<mat4 &>(view));
    shader->setMat4("u_proj", const_cast<mat4 &>(proj));

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL error: " << err << std::endl;
    }
}
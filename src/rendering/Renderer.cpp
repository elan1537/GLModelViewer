// Renderer.cpp
#include <glad/glad.h>

#include "Renderer.h"
#include <iostream>

Renderer::Renderer() {}
Renderer::~Renderer() {}

bool Renderer::Init()
{
    glEnable(GL_PROGRAM_POINT_SIZE);
    return true;
}

void Renderer::Begin()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::End()
{

    // 렌더링 마무리 작업
}
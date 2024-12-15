// Renderer.cpp
#include <glad/glad.h>

#include "Renderer.h"
#include <iostream>

Renderer::Renderer() {}
Renderer::~Renderer() {}

bool Renderer::Init()
{
    return true;
}

void Renderer::Begin()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Renderer::End()
{

    // 렌더링 마무리 작업
}
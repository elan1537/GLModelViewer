// Renderer.h
#pragma once

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Init();
    void Begin();
    void End();

private:
    // Framebuffer, VAO/VBO, Shader, State 설정 등
};
// ImGuiLayer.h
#pragma once
struct GLFWwindow;

class ImGuiLayer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    bool Init(GLFWwindow* window);
    void Begin();
    void End();
};

#include "ImGuiLayer.h"
#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <imgui_internal.h>
#include <GLFW/glfw3.h>

#include <iostream>
ImGuiLayer::ImGuiLayer() {}
ImGuiLayer::~ImGuiLayer() {}

bool ImGuiLayer::Init(GLFWwindow *window)
{
    // std::cout << "ImGuiLayer::Init" << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true); // glfw 창, OpenGL용 초기화
    ImGui_ImplOpenGL3_Init("#version 330");     // 사용하려는 GLSL 버전에 맞게
    return true;
}

void ImGuiLayer::Begin()
{
    // std::cout << "ImGuiLayer::Begin" << std::endl;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    // std::cout << "ImGuiLayer::End" << std::endl;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
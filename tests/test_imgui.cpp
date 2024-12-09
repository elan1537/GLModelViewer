#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main()
{
    // GLFW 초기화
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 윈도우 생성
    GLFWwindow *window = glfwCreateWindow(1280, 720, "ImGui Test", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLEW 초기화
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed!" << std::endl;
        return -1;
    }

    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 테스트용 변수들
    float f = 0.0f;
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        // ImGui 프레임 시작
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui 데모 윈도우 (테스트용)
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 커스텀 ImGui 윈도우
        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("This is a test window.");
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::SliderFloat("Float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("Clear color", (float *)&clear_color);
        ImGui::End();

        // 렌더링
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // ImGui 렌더링
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 버퍼 스왑 및 이벤트 처리
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ImGui 정리
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // GLFW 종료
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
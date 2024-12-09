#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Dataset.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main()
{
    Dataset dataset("../model/precitec");
    const std::vector<CameraInfo> &cameras = dataset.getCameras();

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Camera Viewer", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    glfwSwapInterval(1);

    if (!cameras.empty())
    {
        dataset.loadCameraImage(0);
        const_cast<CameraInfo &>(cameras[0]).createTexture();
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Camera View");
        
        if (!cameras.empty())
        {
            const CameraInfo& camera = cameras[0];
            ImVec2 imageSize(camera.width, camera.height);

            ImVec2 availableSize = ImGui::GetContentRegionAvail();
            float scale = availableSize.x / imageSize.x;
            imageSize.x *= scale;
            imageSize.y *= scale;
        
            std::cout << imageSize.x << " " << imageSize.y << std::endl;
            ImGui::Image((ImTextureID)(intptr_t)camera.textureID, ImVec2(imageSize.x, imageSize.y));
        }
        else 
        {
            ImGui::Text("No camera available");
        }
        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

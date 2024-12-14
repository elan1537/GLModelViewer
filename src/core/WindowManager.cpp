#include <glad/glad.h>

#include "WindowManager.h"
#include <iostream>
#include "core/InputHandler.h"

WindowManager::WindowManager() : m_Window(nullptr) {}

WindowManager::~WindowManager()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}

bool WindowManager::Init(int width, int height, const char *title)
{
    std::cout << "WindowManager::Init" << std::endl;
    if (!glfwInit())
        return false;

    // OpenGL 버전 설정 (예: 3.3 core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_Window)
        return false;
    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        return false;
    }

    glfwSwapInterval(1);

    InputHandler::Init(m_Window);
    return true;
}

void WindowManager::PollEvents()
{
    glfwPollEvents();
}

void WindowManager::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

bool WindowManager::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}
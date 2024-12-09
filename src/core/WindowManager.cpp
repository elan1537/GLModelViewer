#include "WindowManager.h"
#include <GLFW/glfw3.h>

WindowManager::WindowManager() : m_Window(nullptr) {}
WindowManager::~WindowManager() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}

bool WindowManager::Init(int width, int height, const char* title) {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_Window) return false;
    glfwMakeContextCurrent(m_Window);

    glfwSwapInterval(1);
    return true;
}

void WindowManager::PoolEvents() {
    glfwPollEvents();
}

void WindowManager::SwapBuffers() {
    glfwSwapBuffers(m_Window);
}

bool WindowManager::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}
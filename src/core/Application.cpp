#include "Application.h"
#include "WindowManager.h"

#include <iostream>

Application::Application() : m_Running(false) {}
Application::~Application() {}

bool Application::Init() {
    if (!InitWindow()) {
        std::cerr << "Failed to initialize window.\n";
        return false;
    }

    if (!InitRenderer()) {
        std::cerr << "Failed to initialize renderer.\n";
        return false;
    }

    if (!InitGUI()) {
        std::cerr << "Failed to initialize GUI.\n";
        return false;
    }

    if (!LoadData()) {
        std::cerr << "Failed to load data.\n";
        return false;
    }

    return true;
}

void Application::Run() {
    while (m_Running && !m_WindowManager->ShouldClose()) {
        m_WindowManager->PoolEvents();
        Update();
        Render();
        m_WindowManager->SwapBuffers();
    }
}

bool Application::InitWindow() {
    m_WindowManager = std::make_unique<WindowManager>();
    if (!m_WindowManager->Init()) {
        std::cerr << "Failed to initialize window manager" << std::endl;
        return false;
    }

    return true;
}
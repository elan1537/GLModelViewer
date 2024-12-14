#include "InputHandler.h"
#include <iostream>

bool InputHandler::s_Keys[1024];
bool InputHandler::s_MouseButtons[8];
double InputHandler::s_MouseX = 0.0;
double InputHandler::s_MouseY = 0.0;

void InputHandler::Init(GLFWwindow *window)
{
    std::cout << "InputHandler::Init" << std::endl;
    // GLFW 콜백 설정
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // 필요하다면 glfwSetScrollCallback 등 추가 가능
}

void InputHandler::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            s_Keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_Keys[key] = false;
        }
    }
}

void InputHandler::MouseMoveCallback(GLFWwindow *window, double xpos, double ypos)
{
    s_MouseX = xpos;
    s_MouseY = ypos;
    // std::cout << "Mouse Move: (" << xpos << ", " << ypos << ")\n";
}

void InputHandler::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button >= 0 && button < 8)
    {
        if (action == GLFW_PRESS)
        {
            s_MouseButtons[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_MouseButtons[button] = false;
        }
    }
}

bool InputHandler::IsKeyPressed(int key)
{
    if (key >= 0 && key < 1024)
        return s_Keys[key];
    return false;
}

bool InputHandler::IsMouseButtonPressed(int button)
{
    if (button >= 0 && button < 8)
        return s_MouseButtons[button];
    return false;
}

double InputHandler::GetMouseX()
{
    return s_MouseX;
}

double InputHandler::GetMouseY()
{
    return s_MouseY;
}
#include "InputHandler.h"
#include <iostream>

bool InputHandler::s_Keys[1024];
bool InputHandler::s_MouseButtons[8];
double InputHandler::s_MouseX = 0.0;
double InputHandler::s_MouseY = 0.0;
float InputHandler::s_ScrollOffset = 0.0f;

void InputHandler::Init(GLFWwindow *window)
{
    std::cout << "InputHandler::Init" << std::endl;
    // GLFW 콜백 설정
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

float InputHandler::GetScrollOffset()
{
    float offset = s_ScrollOffset;
    s_ScrollOffset = 0.0f; // 한번 읽었으면 리셋(혹은 누적 값을 유지하고 싶다면 이 부분은 상황에 맞게 변경)
    return offset;
}

// GLFW scroll callback에서 s_ScrollOffset 업데이트
void InputHandler::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    s_ScrollOffset += (float)yoffset;
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
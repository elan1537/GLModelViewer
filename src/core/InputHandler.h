#pragma once
#include <GLFW/glfw3.h>

class InputHandler
{
public:
    // 키 입력 상태나 마우스 상태를 저장할 정적/전역 변수들
    // 여기서는 간단히 정적 함수 콜백만 구현
    static void Init(GLFWwindow *window);

    // 키보드 콜백
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    // 마우스 이동 콜백
    static void MouseMoveCallback(GLFWwindow *window, double xpos, double ypos);
    // 마우스 버튼 콜백
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    // 필요하다면 키, 마우스 상태를 조회하는 함수
    static bool IsKeyPressed(int key);
    static bool IsMouseButtonPressed(int button);
    static double GetMouseX();
    static double GetMouseY();

    static bool s_Keys[1024];      // 예제: 최대 1024 키 상태 저장
    static bool s_MouseButtons[8]; // 예제: 8개 마우스 버튼 상태
    static double s_MouseX, s_MouseY;
};
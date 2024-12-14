#pragma once

struct GLFWwindow;

class WindowManager {
    public:
        WindowManager();
        ~WindowManager();

        bool Init(int width, int height, const char* title);
        void PollEvents();
        void SwapBuffers();
        bool ShouldClose() const;

        GLFWwindow* GetNativeWindow() const { return m_Window; }

    private:
    GLFWwindow* m_Window;
};
#pragma once

#include <memory>

class WindowManager;
class Renderer;
class ImGuiLayer;
class DataLoader;
class GaussianRenderer;
class CubeRenderer;
// class GaussianVisualizer;
// class MetricVisualizer;
// class ParameterEditor;
class Camera;
class CameraInfo;
// class CameraPath;
// class MetricData;

class Application
{
    const static int WINDOW_WIDTH = 1920;
    const static int WINDOW_HEIGHT = 1080;

public:
    Application();
    ~Application();

    bool Init();
    void Run();
    void Shutdown();
    void UpdateCamera();

private:
    bool InitWindow();
    bool InitRenderer();
    bool InitGUI();
    bool LoadData();

    int m_currentCameraIndex;

    std::unique_ptr<WindowManager> m_WindowManager;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

    std::unique_ptr<DataLoader> m_DataLoader;
    std::unique_ptr<GaussianRenderer> m_GaussianRenderer;
    std::unique_ptr<CubeRenderer> m_CubeRenderer;

    std::unique_ptr<Camera> m_Camera;

    // std::unique_ptr<GaussianVisualizer> m_GaussianVis;
    // std::unique_ptr<MetricVisualizer> m_MetricVis;
    // std::unique_ptr<ParameterEditor> m_ParamEditor;

    // std::unique_ptr<Camera> m_Camera;
    // std::unique_ptr<CameraPath> m_CameraPath;
    // std::unique_ptr<MetricData> m_MetricData;

    bool m_Running;
};
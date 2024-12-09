#pragma once

#include <memory>

class WindowManager;
class Renderer;
class ImGuiLayer;
class DataLoader;
class GaussianVisualizer;
class MetricVisualizer;
class ParameterEditor;
class Camera;
class CameraPath;
class MetricData;

class Application {
public:
    Application();
    ~Application();

    bool Init();
    void Run();
    void Shutdown();

private:
    bool InitWindow();
    bool InitRenderer();
    bool InitGUI();
    bool LoadData();

    std::unique_ptr<WindowManager> m_WindowManager;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

    std::unique_ptr<DataLoader> m_DataLoader;
    std::unique_ptr<GaussianVisualizer> m_GaussianVis;
    std::unique_ptr<MetricVisualizer> m_MetricVis;
    std::unique_ptr<ParameterEditor> m_ParamEditor;

    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<CameraPath> m_CameraPath;
    std::unique_ptr<MetricData> m_MetricData;

    bool m_Running;
};
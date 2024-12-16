#include "Application.h"
#include "WindowManager.h"
#include "gui/ImGuiLayer.h"
#include "gui/LayoutManager.h"
#include "gui/SceneWindow.h"
#include "gui/TrainPathWindow.h"
#include "InputHandler.h"

#include "rendering/Renderer.h"
#include "core/Camera.h"

#include "data_processing/DataLoader.h"
#include "rendering/GaussianRenderer.h"
#include "rendering/CubeRenderer.h"

#include <iostream>
#include "utils/utils.h"

Application::Application() : m_Running(false) {}
Application::~Application() {}

vec2 previous_pan_pos;
vec2 previous_mouse_pos;

bool Application::Init()
{
    if (!InitWindow())
    {
        std::cerr << "Failed to initialize window.\n";
        return false;
    }

    if (!InitRenderer())
    {
        std::cerr << "Failed to initialize renderer.\n";
        return false;
    }

    if (!InitGUI())
    {
        std::cerr << "Failed to initialize GUI.\n";
        return false;
    }

    if (!LoadData())
    {
        std::cerr << "Failed to loadData\n";
    }

    m_Camera = std::make_unique<Camera>();
    m_GaussianRenderer = std::make_unique<GaussianRenderer>();
    m_GaussianRenderer->Init(m_DataLoader->getSplats());

    m_CubeRenderer = std::make_unique<CubeRenderer>();

    std::vector<mat4> poses;
    for (auto &camera : m_DataLoader->getCameras())
    {
        mat4 pose = camera.getPose();
        poses.push_back(pose);
    }

    m_CubeRenderer->Init(poses);

    m_Running = true;
    return true;
}

void Application::UpdateCamera()
{
    // Step 1: Calculate forward, right, up
    vec3 forward = normalize(m_Camera->target - m_Camera->position);
    vec3 right = normalize(cross(forward, m_Camera->up));
    vec3 up = cross(right, forward);
    m_Camera->up = normalize(up); // up 벡터 정규화 (ensure orthogonality)

    float moveSpeed = 0.06f;
    float rotateSpeed = 1.0f;
    float zoomSpeed = 0.1f;
    float panSpeed = 0.1f;

    // Step 2: Keyboard movement (W,A,S,D)
    if (InputHandler::IsKeyPressed(GLFW_KEY_W))
    {
        m_Camera->position += forward * moveSpeed;
        m_Camera->target += forward * moveSpeed;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_S))
    {
        m_Camera->position -= forward * moveSpeed;
        m_Camera->target -= forward * moveSpeed;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_A))
    {
        m_Camera->position -= right * moveSpeed;
        m_Camera->target -= right * moveSpeed;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_D))
    {
        m_Camera->position += right * moveSpeed;
        m_Camera->target += right * moveSpeed;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_Q))
    {
        m_Camera->position -= up * moveSpeed;
        m_Camera->target -= up * moveSpeed;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_E))
    {
        m_Camera->position += up * moveSpeed;
        m_Camera->target += up * moveSpeed;
    }

    // 현재 마우스 위치 정규화
    vec2 t_mousePos = vec2(InputHandler::GetMouseX(), InputHandler::GetMouseY());
    vec2 current_mouse_pos = vec2(
        (t_mousePos.x - WINDOW_WIDTH / 2) * 2.0f / WINDOW_WIDTH,
        (t_mousePos.y - WINDOW_HEIGHT / 2) * 2.0f / WINDOW_HEIGHT);

    if (InputHandler::s_MouseButtons[GLFW_MOUSE_BUTTON_LEFT])
    {
        if (current_mouse_pos != previous_mouse_pos)
        {
            float z0 = utils::getZFromXY(previous_mouse_pos);
            float z1 = utils::getZFromXY(current_mouse_pos);

            vec3 p0(previous_mouse_pos.x, previous_mouse_pos.y, z0);
            vec3 p1(current_mouse_pos.x, current_mouse_pos.y, z1);

            vec3 axis = cross(p0, p1);
            float d = dot(p0, p1);
            d = std::max(-1.0f, std::min(1.0f, d));
            float angle = acos(d) * rotateSpeed;

            if (length(axis) > 1e-6f && fabs(angle) > 1e-6f)
            {
                axis = normalize(axis);

                // 회전은 타겟을 중심으로 카메라 position을 회전하는 방식으로 변경
                vec3 camToTarget = m_Camera->position - m_Camera->target;
                mat3 R = utils::RotateMatrix(axis, angle);
                camToTarget = R * camToTarget;
                m_Camera->position = m_Camera->target + camToTarget;

                // up 벡터도 회전
                m_Camera->up = R * m_Camera->up;
            }

            previous_mouse_pos = current_mouse_pos;
        }
    }
    else
    {
        previous_mouse_pos = current_mouse_pos;
    }

    // Step 4: Mouse Zoom (Scroll)
    float scroll = InputHandler::GetScrollOffset();
    if (scroll != 0.0f)
    {
        // forward 방향으로 zoom
        vec3 dir = normalize(m_Camera->target - m_Camera->position);
        float distance = scroll * zoomSpeed;
        m_Camera->position += dir * distance;
        // m_Camera->target += dir * distance;
    }

    // Step 5: Mouse Pan (e.g. Middle mouse drag)
    if (InputHandler::s_MouseButtons[GLFW_MOUSE_BUTTON_MIDDLE])
    {
        vec2 mouseDelta = (current_mouse_pos - previous_pan_pos) * panSpeed;
        // 마우스 움직임에 따라 right, up 방향으로 이동
        m_Camera->position += (right * mouseDelta.x + up * mouseDelta.y);
        m_Camera->target += (right * mouseDelta.x + up * mouseDelta.y);
    }
    previous_pan_pos = current_mouse_pos;

    // Step 6: Recompute view matrix
    m_Camera->pose = LookAt(m_Camera->position, m_Camera->target, m_Camera->up);
}

vec3 rotmat2euler(const mat3 &R)
{
    return vec3(atan2(R[2][1], R[2][2]) * 180.0f / M_PI,
                atan2(-R[2][0], sqrt(R[2][1] * R[2][1] + R[2][2] * R[2][2])) * 180.0f / M_PI,
                atan2(R[1][0], R[0][0]) * 180.0f / M_PI);
}

void Application::Run()
{
    int currentCameraIndex = 0;
    auto cameras = m_DataLoader->getCameras();
    std::cout << "Number of cameras: " << cameras.size() << std::endl;
    cameras[currentCameraIndex].loadImage();
    static bool layoutInitialized = false;

    while (m_Running && !m_WindowManager->ShouldClose())
    {
        if (InputHandler::IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            m_WindowManager->ShouldClose();
            m_Running = false;
        }
        UpdateCamera();

        m_Renderer->Begin();
        {
            // 첫 번째 뷰포트 설정
            glViewport(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
            float aspect_top = (float)WINDOW_WIDTH / (float)(WINDOW_HEIGHT);

            // 첫 번째 뷰포트의 카메라 설정 (예: 현재 m_Camera)
            mat4 proj = Perspective(30.0f, aspect_top, 0.01f, 100.0f);

            m_GaussianRenderer->Draw(m_Camera->pose, proj, vec4(0, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT), vec2(0.01f, 100.0f));
            m_CubeRenderer->Draw(mat4(1.0f), m_Camera->pose, proj);

            // 두 번째 뷰포트(하단)
            glViewport(0, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
            float aspect_bottom = (float)WINDOW_WIDTH / (float)(WINDOW_HEIGHT);

            // 두 번째 뷰포트용 카메라 설정
            mat4 t = cameras[currentCameraIndex].getPose();
            mat3 R = mat3(t[0][0], t[0][1], t[0][2],
                          t[1][0], t[1][1], t[1][2],
                          t[2][0], t[2][1], t[2][2]);
            vec3 t_pos = vec3(t[0][3], t[1][3], t[2][3]);

            // 카메라 전방 벡터 추출 (예: R의 z축 사용)
            vec3 forward(R[2][0], R[2][1], R[2][2]);
            mat4 altView = LookAt(t_pos, t_pos + forward, vec3(0, 1, 0));
            mat4 altProj = Perspective(30.0f, aspect_bottom, 0.01f, 100.0f);

            m_GaussianRenderer->Draw(altView, altProj, vec4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), vec2(0.01f, 100.0f));
            m_CubeRenderer->Draw(mat4(1.0f), altView, altProj);
        }
        m_Renderer->End();

        m_ImGuiLayer->Begin();
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        {

            ImGui::Begin("Camera");
            ImGui::Text("Position: %f, %f, %f", m_Camera->position.x, m_Camera->position.y, m_Camera->position.z);
            ImGui::Text("Target: %f, %f, %f", m_Camera->target.x, m_Camera->target.y, m_Camera->target.z);
            ImGui::Text("Up: %f, %f, %f", m_Camera->up.x, m_Camera->up.y, m_Camera->up.z);
            ImGui::End();

            ImGui::Begin("Train Camera");
            ImGui::Text("Train camera points/images here.");
            ImGui::Image((ImTextureID)(intptr_t)cameras[currentCameraIndex].getTextureID(), ImVec2(200, 200));

            if (ImGui::Button("Next Camera"))
            {
                cameras[currentCameraIndex].unloadImage();
                currentCameraIndex++;
                if (currentCameraIndex >= cameras.size())
                    currentCameraIndex = 0;
                cameras[currentCameraIndex].loadImage();
                m_CubeRenderer->SetSelectedIndex(currentCameraIndex);
            }
            ImGui::SameLine();
            if (ImGui::Button("Previous Camera"))
            {
                cameras[currentCameraIndex].unloadImage();
                currentCameraIndex--;
                if (currentCameraIndex < 0)
                    currentCameraIndex = cameras.size() - 1;
                cameras[currentCameraIndex].loadImage();
                m_CubeRenderer->SetSelectedIndex(currentCameraIndex);
            }

            mat4 t = cameras[currentCameraIndex].getPose();
            mat3 R = mat3(t[0][0], t[0][1], t[0][2],
                          t[1][0], t[1][1], t[1][2],
                          t[2][0], t[2][1], t[2][2]);

            vec3 t_pos = vec3(t[0][3], t[1][3], t[2][3]);
            vec3 t_euler = rotmat2euler(R);
            ImGui::Text("Pose: %f, %f, %f", t_pos[0], t_pos[1], t_pos[2]);
            ImGui::Text("Euler: %f, %f, %f", t_euler[0], t_euler[1], t_euler[2]);

            ImGui::End();

            ImGui::Begin("Test Path");
            ImGui::Text("Test camera points/images here.");
            ImGui::Image((ImTextureID)(intptr_t)cameras[currentCameraIndex].getTextureID(), ImVec2(200, 200));

            if (ImGui::Button("Next Camera"))
            {
                cameras[currentCameraIndex].unloadImage();
                currentCameraIndex++;
                if (currentCameraIndex >= cameras.size())
                    currentCameraIndex = 0;
                cameras[currentCameraIndex].loadImage();
            }
            ImGui::SameLine();
            if (ImGui::Button("Previous Camera"))
            {
                cameras[currentCameraIndex].unloadImage();
                currentCameraIndex--;
                if (currentCameraIndex < 0)
                    currentCameraIndex = cameras.size() - 1;
                cameras[currentCameraIndex].loadImage();
            }

            ImGui::End();
        }
        m_ImGuiLayer->End();

        m_WindowManager->SwapBuffers();
        m_WindowManager->PollEvents();
    }
}

void Application::Shutdown()
{
    // Clean-up resources
    m_ImGuiLayer.reset();
    // m_GaussianVis.reset();
    // m_MetricVis.reset();
    // m_ParamEditor.reset();
    // m_MetricData.reset();
    // m_CameraPath.reset();
    // m_Camera.reset();
    // m_DataLoader.reset();
    // m_Renderer.reset();
    m_WindowManager.reset();
}

bool Application::InitWindow()
{
    m_WindowManager = std::make_unique<WindowManager>();
    return m_WindowManager->Init(WINDOW_WIDTH, WINDOW_HEIGHT, "Gaussian Splatting Visualizer");
}

bool Application::InitRenderer()
{
    m_Renderer = std::make_unique<Renderer>();
    return m_Renderer->Init();
}

bool Application::InitGUI()
{
    m_ImGuiLayer = std::make_unique<ImGuiLayer>();
    return m_ImGuiLayer->Init(m_WindowManager->GetNativeWindow());
}

bool Application::LoadData()
{
    m_DataLoader = std::make_unique<DataLoader>("../model/precitec");
    return true;
}
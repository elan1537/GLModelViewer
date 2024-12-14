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

    std::vector<vec3> positions;
    for (auto &camera : m_DataLoader->getCameras())
    {
        vec3 pos = vec3(camera.getPose()[0][3], camera.getPose()[1][3], camera.getPose()[2][3]);
        positions.push_back(pos);
    }

    m_CubeRenderer->Init(positions);

    m_Running = true;
    return true;
}

void Application::UpdateCamera()
{
    // forward, right 벡터 계산
    vec3 forward = normalize(m_Camera->target - m_Camera->position);
    vec3 right = normalize(cross(forward, m_Camera->up));
    // 필요한 경우 up 벡터 재정리:
    // vec3 up = cross(right, forward);
    // m_Camera->up = up; // up을 재계산해서 유지하고 싶다면

    if (InputHandler::IsKeyPressed(GLFW_KEY_W))
    {
        m_Camera->position += forward * 0.06f;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_A))
    {
        m_Camera->position -= right * 0.06f; // 왼쪽이므로 right의 반대방향
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_S))
    {
        m_Camera->position -= forward * 0.06f;
    }
    if (InputHandler::IsKeyPressed(GLFW_KEY_D))
    {
        m_Camera->position += right * 0.06f; // 오른쪽 방향 이동
    }

    // 현재 마우스 위치 정규화
    vec2 t_mousePos = vec2(InputHandler::GetMouseX(), InputHandler::GetMouseY());
    vec2 current_mouse_pos = vec2(
        (t_mousePos.x - WINDOW_WIDTH / 2) * 2.0f / WINDOW_WIDTH,
        (t_mousePos.y - WINDOW_HEIGHT / 2) * 2.0f / WINDOW_HEIGHT);

    if (InputHandler::s_MouseButtons[GLFW_MOUSE_BUTTON_LEFT])
    {
        // 마우스가 움직였을 때만 회전 계산
        if (current_mouse_pos != previous_mouse_pos)
        {
            float z0 = utils::getZFromXY(previous_mouse_pos);
            float z1 = utils::getZFromXY(current_mouse_pos);

            vec3 p0(previous_mouse_pos.x, previous_mouse_pos.y, z0);
            vec3 p1(current_mouse_pos.x, current_mouse_pos.y, z1);

            vec3 axis = cross(p0, p1);
            float d = dot(p0, p1);
            d = std::max(-1.0f, std::min(1.0f, d));
            float angle = acos(d);

            if (length(axis) > 1e-6f && fabs(angle) > 1e-6f)
            {
                vec3 k = normalize(axis);
                float x = k.x;
                float y = k.y;
                float z = k.z;
                float c = cos(angle);
                float s = sin(angle);
                float one_c = 1.0f - c;

                mat3 R(1.0f);
                R[0][0] = c + x * x * one_c;
                R[0][1] = x * y * one_c - z * s;
                R[0][2] = x * z * one_c + y * s;

                R[1][0] = y * x * one_c + z * s;
                R[1][1] = c + y * y * one_c;
                R[1][2] = y * z * one_c - x * s;

                R[2][0] = z * x * one_c - y * s;
                R[2][1] = z * y * one_c + x * s;
                R[2][2] = c + z * z * one_c;

                // 회전 적용
                forward = normalize(R * forward);
                m_Camera->up = normalize(R * m_Camera->up);
            }

            // 회전 적용 후 현재 마우스 위치를 이전 위치로 갱신
            previous_mouse_pos = current_mouse_pos;
        }
    }
    else
    {
        // 마우스 버튼이 눌리지 않은 상태에서도 previous를 업데이트
        previous_mouse_pos = current_mouse_pos;
    }

    // 카메라 뷰 행렬 업데이트

    m_Camera->target = m_Camera->position + forward;
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

    int winWidth = 800;
    int winHeight = 600;

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
            // // 카메라 위치를 조정
            // mat4 view = LookAt(m_Camera->position, m_Camera->target, m_Camera->up);
            // mat4 proj = Perspective(30.0f,           // FOV (시야각)
            //                         800.0f / 600.0f, // 종횡비
            //                         0.01f,           // near plane
            //                         100.0f);

            // m_GaussianRenderer->Draw(mat4(1.0f), view, proj);
            // m_CubeRenderer->Draw(mat4(1.0f), view, proj);

            // 화면 클리어(모든 뷰포트 렌더링 전에)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 첫 번째 뷰포트 설정
            glViewport(0, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);

            // 첫 번째 뷰포트의 카메라 설정 (예: 현재 m_Camera)
            mat4 view = LookAt(m_Camera->position, m_Camera->target, m_Camera->up);
            mat4 proj = Perspective(30.0f, (winWidth / 2.0f) / (float)winHeight, 0.01f, 100.0f);

            m_GaussianRenderer->Draw(mat4(1.0f), view, proj);

            // 두 번째 뷰포트 설정
            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

            // 두 번째 뷰포트용 카메라 설정
            // 예를 들어 cameras[currentCameraIndex]를 이용해 다른 시점 제공
            mat4 t = cameras[currentCameraIndex].getPose();
            mat3 R = mat3(t[0][0], t[0][1], t[0][2],
                          t[1][0], t[1][1], t[1][2],
                          t[2][0], t[2][1], t[2][2]);
            vec3 t_pos = vec3(t[0][3], t[1][3], t[2][3]);
            // 여기서 t_pos와 R을 사용해 view 행렬을 생성 (적절한 LookAt 변환)
            // 카메라의 위치: t_pos, 방향: R 이용하거나 Euler 변환 후 Target 산출
            // 간단히 말해 t_pos를 카메라 위치로 두고, R에서 바라보는 방향을 추출
            // (아래는 단순예로, 본인이 구현한 rotmat2euler 등을 활용)

            vec3 forward(R[2][0], R[2][1], R[2][2]); // ROT 행렬에서 z축 방향(또는 다른 축) 사용
            // 카메라가 바라보는 위치를 t_pos + forward로 가정
            mat4 altView = LookAt(t_pos, t_pos + forward, vec3(0, 1, 0));
            // 투영행렬도 다른 종횡비에 맞출 수 있음. 여기서는 동일하게 사용
            mat4 altProj = Perspective(30.0f, (winWidth / 2.0f) / (float)winHeight, 0.01f, 100.0f);

            m_GaussianRenderer->Draw(mat4(1.0f), altView, altProj);
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
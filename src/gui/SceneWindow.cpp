#include "SceneWindow.h"
#include <imgui.h>

namespace SceneWindow
{
    void Init()
    {
    }

    void Draw()
    {
        ImGui::Begin("Scene Window");
        ImGui::Text("3D Scene: Gaussian Splatting");
        ImGui::End();
    }
}
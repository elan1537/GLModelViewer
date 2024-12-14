#include "TrainPathWindow.h"
#include <imgui.h>

namespace TrainPathWindow
{

    ImageControl Draw(GLuint texID, int img_w, int img_h)
    {
        ImageControl ctrl = {false, false};

        ImGui::Begin("Train Path");
        ImGui::Text("Train path points/images here.");
        if (texID != 0 && img_w > 0 && img_h > 0)
        {
            float aspect = (float)img_w / (float)img_h;
            float maxWidth = 300.0f; // 최대 폭
            float displayWidth = maxWidth;
            float displayHeight = displayWidth / aspect;

            // 만약 높이가 너무 클 경우 높이를 300으로 고정하고 폭을 재조정할 수도 있다.
            // if (displayHeight > 300.0f) {
            //     displayHeight = 300.0f;
            //     displayWidth = displayHeight * aspect;
            // }

            ImGui::Image((ImTextureID)(intptr_t)texID, ImVec2(displayWidth, displayHeight));
        }
        else
        {
            ImGui::Text("No image loaded.");
        }

        if (ImGui::Button("Next Image"))
        {
            ctrl.next = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Previous Image"))
        {
            ctrl.prev = true;
        }
        ImGui::End();
        return ctrl;
    }
}
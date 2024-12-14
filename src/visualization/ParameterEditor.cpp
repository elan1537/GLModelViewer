#include "ParameterEditor.h"
#include <imgui.h>
#include "visualization/GaussianVisualizer.h"
#include "visualization/MetricVisualizer.h"

#include <iostream>

ParameterEditor::ParameterEditor(GaussianVisualizer *gv, MetricVisualizer *mv)
    : m_GaussianVis(gv), m_MetricVis(mv), m_Alpha(1.0f)
{
    m_ColorRange[0] = 0.0f;
    m_ColorRange[1] = 1.0f;
}

void ParameterEditor::DrawUI()
{
    std::cout << "ParameterEditor::DrawUI" << std::endl;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 500));

    ImGui::Begin("Parameter Editor", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("This panel is fixed at the asdfasdf");
    // if (ImGui::SliderFloat("Alpha", &m_Alpha, 0.0f, 1.0f))
    // {
    //     std::cout << "Alpha: " << m_Alpha << std::endl;
    // m_GaussianVis->SetTransparency(m_Alpha);
    // }
    // if (ImGui::SliderFloat2("ColorRange", m_ColorRange, 0.0f, 10.0f))
    // {
    // std::cout << "ColorRange: " << m_ColorRange[0] << ", " << m_ColorRange[1] << std::endl;
    // m_GaussianVis->SetColorMapRange(m_ColorRange[0], m_ColorRange[1]);
    // }
    ImGui::End();
}
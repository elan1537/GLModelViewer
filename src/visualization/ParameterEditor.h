#pragma once

class GaussianVisualizer;
class MetricVisualizer;

class ParameterEditor {
public:
    ParameterEditor(GaussianVisualizer* gv, MetricVisualizer* mv);
    void DrawUI();

private:
    GaussianVisualizer* m_GaussianVis;
    MetricVisualizer* m_MetricVis;
    float m_Alpha;
    float m_ColorRange[2];
};
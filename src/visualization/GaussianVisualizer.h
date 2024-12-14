#pragma once

#include <vector>

class DataLoader;

class GaussianVisualizer
{
public:
    GaussianVisualizer(DataLoader *loader);
    ~GaussianVisualizer();

    void Render();
    void SetTransparency(float alpha);
    void SetColorMapRange(float minVal, float maxVal);

private:
    DataLoader *m_Loader;
    float m_Alpha;
    float m_ColorMin, m_ColorMax;
};
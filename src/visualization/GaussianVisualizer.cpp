#include "GaussianVisualizer.h"
#include <iostream>

GaussianVisualizer::GaussianVisualizer(DataLoader* loader) : m_Loader(loader) {}
GaussianVisualizer::~GaussianVisualizer() {}

void GaussianVisualizer::Render() {
    std::cout << "GaussianVisualizer::Render" << std::endl;
}

void GaussianVisualizer::SetTransparency(float alpha) {
    m_Alpha = alpha;
}

void GaussianVisualizer::SetColorMapRange(float minVal, float maxVal) {
    m_ColorMin = minVal;
    m_ColorMax = maxVal;
}
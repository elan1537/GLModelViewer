#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "utils/Angel.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

// SH 구조체
template <int D>
struct SH
{
    float shs[(D + 1) * (D + 1) * 3];
};

// GSPoint 구조체
struct GSPoint
{
    vec3 position;
    vec3 normal;
    SH<3> shs;
    vec4 color;
    float opacity;
    vec3 scale;
    vec4 rotation;
};

class SplatData
{
public:
    SplatData(const std::string &projectPath);

    bool loadGaussiansFromPLY(const std::string &plyFile);
    const std::vector<GSPoint> &getGSPoints() const { return m_gsPoints; }
    bool isValid() const { return m_valid; }
    int getNumGaussians() const { return m_numGaussians; }
    const vec3 &getCenter() const { return m_center; }

private:
    std::string m_projectPath;
    std::vector<GSPoint> m_gsPoints;

    bool m_valid = false;
    int m_numGaussians = 0;
    vec3 m_center = vec3(0.0f);

    static float sigmoid(float x)
    {
        return 1.0f / (1.0f + std::exp(-x));
    }
};

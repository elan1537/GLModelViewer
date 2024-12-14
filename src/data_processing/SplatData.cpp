#include "SplatData.h"

SplatData::SplatData(const std::string &projectPath)
    : m_projectPath(projectPath) {}

bool SplatData::loadGaussiansFromPLY(const std::string &plyFile)
{
    m_center = vec3(0.0f);
    m_numGaussians = 0;
    m_gsPoints.clear();
    m_valid = false;

    std::string fullPath = m_projectPath + "/" + plyFile;
    std::ifstream reader(fullPath, std::ios::binary);
    if (!reader.good())
    {
        std::cerr << "Bad PLY reader: " << fullPath << "?" << std::endl;
        return false;
    }

    std::string buf, dummy;

    // PLY 헤더 파싱 (간단히 가정)
    std::getline(reader, buf); // 'ply'
    std::getline(reader, buf); // format ...
    std::getline(reader, buf); // element vertex X
    {
        std::stringstream ss(buf);
        ss >> dummy >> dummy >> m_numGaussians; // element vertex X
    }

    // end_header까지 스킵
    while (std::getline(reader, dummy))
    {
        if (dummy.compare("end_header") == 0)
            break;
    }

    if (m_numGaussians <= 0)
    {
        std::cerr << "No splats in file.\n";
        return false;
    }

    m_gsPoints.resize(m_numGaussians);

    // 속성 순서(예시):
    // x,y,z, nx,ny,nz, f_dc_0,f_dc_1,f_dc_2, f_rest_0..f_rest_44(45 floats), opacity, scale_0..2, rot_0..3
    // 총 float 개수: 3(pos) +3(normal) +3(dc) +45(rest) +1(opacity) +3(scale) +4(rot) = 62 floats per point

    for (int i = 0; i < m_numGaussians; i++)
    {
        GSPoint &p = m_gsPoints[i];

        // position
        float x, y, z;
        reader.read((char *)&x, sizeof(float));
        reader.read((char *)&y, sizeof(float));
        reader.read((char *)&z, sizeof(float));
        p.position = vec3(x, y, z);

        // normal
        float nx, ny, nz;
        reader.read((char *)&nx, sizeof(float));
        reader.read((char *)&ny, sizeof(float));
        reader.read((char *)&nz, sizeof(float));
        p.normal = vec3(nx, ny, nz);

        // SH: f_dc_0..2(3 floats) + f_rest_0..44(45 floats) 총 48 floats
        reader.read((char *)&p.shs.shs[0], sizeof(float) * 48);
        p.color = vec4(p.shs.shs[0], p.shs.shs[1], p.shs.shs[2], 1.0f);

        // opacity
        float opacity;
        reader.read((char *)&opacity, sizeof(float));
        p.opacity = sigmoid(opacity);

        // scale
        float s0, s1, s2;
        reader.read((char *)&s0, sizeof(float));
        reader.read((char *)&s1, sizeof(float));
        reader.read((char *)&s2, sizeof(float));
        p.scale = vec3(exp(s0), exp(s1), exp(s2));

        // rotation (4 floats)
        float r0, r1, r2, r3;
        reader.read((char *)&r0, sizeof(float));
        reader.read((char *)&r1, sizeof(float));
        reader.read((char *)&r2, sizeof(float));
        reader.read((char *)&r3, sizeof(float));
        p.rotation = vec4(r0, r1, r2, r3);

        m_center += p.position;
    }

    if (reader.eof())
    {
        std::cerr << "Reader is EOF?\n";
        m_valid = false;
        return false;
    }

    if (m_numGaussians > 0)
    {
        m_center /= (float)m_numGaussians;
    }

    m_valid = true;
    return true;
}
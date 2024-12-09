#ifndef SPLATREADER_H
#define SPLATREADER_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <Angel.h>
#include <vector>

template <int D>
struct SH
{
    float shs[(D + 1) * (D + 1) * 3];
};

struct GSPoint
{
    vec3 position;
    vec3 normal;
    SH<3> shs;
    float opacity;
    vec3 scale;
    vec3 rotation;
};

struct GSSplats
{
    bool valid;
    int numSplats;
    std::vector<GSPoint> splats;
};

float sigmoid(float x)
{
    return 1.0f / (1.0f + exp(-x));
}

class SplatReader
{
public:
    bool loadFromPly(const std::string &plyPath)
    {
        _center = vec3(0.0f);
        _numGaussians = 0;
        _positions.clear();
        _scales.clear();
        _shs.clear();
        _opacities.clear();
        _valid = false;

        std::unique_ptr<GSSplats> splatPtr = loadFromSplatsPly(plyPath);
        if (!splatPtr->valid)
        {
            return false;
        }

        _numGaussians = splatPtr->numSplats;
        for (const auto &sp : splatPtr->splats)
        {
            _positions.push_back(vec4(sp.position, 1.0f));
            _scales.push_back(vec4(sp.scale, 1.0f));
            _shs.push_back(sp.shs);
            _rotation.push_back(normalize(sp.rotation));
            _opacities.push_back(sigmoid(sp.opacity));

            _center += sp.position;
        }

        if (splatPtr->splats.size() > 0)
        {
            _center /= splatPtr->splats.size();
        }

        _valid = true;
        return true;
    };

    const vec3 &getCenter() const
    {
        return _center;
    }

    int getNumGaussians() const
    {
        return _numGaussians;
    }

    const std::vector<vec4> &getPositions() const
    {
        return _positions;
    }

    const std::vector<SH<3>> &getSH() const
    {
        return _shs;
    }

    const std::vector<vec4> &getRotations() const
    {
        return _rotation;
    }

    const std::vector<float> &getOpacities() const
    {
        return _opacities;
    }

    bool isValid() const
    {
        return _valid;
    }

protected:
    static std::unique_ptr<GSSplats> loadFromSplatsPly(const std::string &path)
    {
        std::unique_ptr<GSSplats> splats = std::make_unique<GSSplats>();
        splats->numSplats = 0;
        splats->valid = false;

        std::ifstream reader(path, std::ios::binary);

        if (!reader.good())
        {
            std::cerr << "Bad PLY reader: " << path << "?" << std::endl;
            return std::move(splats);
        }

        std::string buf;
        std::getline(reader, buf);
        std::getline(reader, buf);
        std::getline(reader, buf);
        std::stringstream ss(buf);
        std::string dummy;

        ss >> dummy >> dummy >> splats->numSplats;
        splats->splats.resize(splats->numSplats);
        std::cout << "Loading " << splats->numSplats << " splats.." << std::endl;

        while (std::getline(reader, dummy))
        {
            if (dummy.compare("end_header") == 0)
                break;
        }
        reader.read((char *)splats->splats.data(), splats->numSplats * sizeof(GSPoint));
        if (reader.eof())
        {
            std::cerr << "Reader is EOF?" << std::endl;
            splats->valid = false;
            return std::move(splats);
        }
        splats->valid = true;

        return std::move(splats);
    };

    vec3 _center;
    int _numGaussians;

    std::vector<vec4> _positions;
    std::vector<vec4> _scales;
    std::vector<SH<3>> _shs;
    std::vector<vec4> _rotation;
    std::vector<float> _opacities;

    bool _valid;
};

#endif
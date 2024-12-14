#pragma once

#include <string>
#include <memory>

#include "SplatData.h"
#include "CameraDataLoader.h"

class DataLoader
{
public:
    DataLoader(const std::string &projectPath);
    ~DataLoader();

    std::vector<CameraInfo> getCameras();
    std::vector<GSPoint> getSplats();
    int m_currentCameraIndex;

private:
    std::string m_projectPath;
    std::unique_ptr<CameraDataLoader> m_CameraDataLoader;
    std::unique_ptr<SplatData> m_SplatData;
    std::vector<CameraInfo> m_Cameras;

    bool LoadCameraData();
    bool LoadSplatData();
};

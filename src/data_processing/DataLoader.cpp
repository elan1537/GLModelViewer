#include "DataLoader.h"

DataLoader::DataLoader(const std::string &projectPath)
    : m_projectPath(projectPath),
      m_CameraDataLoader(std::make_unique<CameraDataLoader>(projectPath)),
      m_SplatData(std::make_unique<SplatData>(projectPath))
{
    LoadCameraData();
    LoadSplatData();
    m_CameraDataLoader->loadCameraImage(0);
    m_currentCameraIndex = 0;

    m_Cameras = m_CameraDataLoader->getCameras();
    std::cout << "Number of cameras: " << m_Cameras.size() << std::endl;
}

DataLoader::~DataLoader() {}

bool DataLoader::LoadCameraData()
{
    m_CameraDataLoader = std::make_unique<CameraDataLoader>(m_projectPath);
    if (!m_CameraDataLoader->loadCameraData())
    {
        std::cerr << "Failed to load camera data.\n";
        return false;
    }
    m_CameraDataLoader->loadCameraImage(0);
    return true;
}

bool DataLoader::LoadSplatData()
{
    m_SplatData->loadGaussiansFromPLY("point_cloud/iteration_30000/point_cloud.ply");
    return true;
}

std::vector<CameraInfo> DataLoader::getCameras()
{
    return m_Cameras;
}

std::vector<GSPoint> DataLoader::getSplats()
{
    return m_SplatData->getGSPoints();
}
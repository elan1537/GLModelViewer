#pragma once

#include <string>
#include <vector>
#include <map>
#include "CameraInfo.h"

class CameraDataLoader
{
public:
    CameraDataLoader(const std::string &projectPath);

    bool loadCameraData();
    std::vector<CameraInfo> &getCameras();
    bool loadCameraImage(size_t index);
    void loadAllImages();

private:
    struct IntrinsicParams
    {
        int32_t model_id;
        std::string model_name;
        uint64_t width;
        uint64_t height;
        std::vector<double> params;

        IntrinsicParams();
        IntrinsicParams(const IntrinsicParams &other);
        IntrinsicParams &operator=(const IntrinsicParams &other);
    };

    std::map<int, IntrinsicParams> m_intrinsics;
    std::vector<CameraInfo> m_cameras;
    std::string m_projectPath;

    void ReadIntrinsicBinary(const char *filePath);
    void ReadExtrinsicBinary(const char *filePath);
};
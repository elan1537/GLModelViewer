
#include "utils/utils.h"
#include "CameraDataLoader.h"

#include <fstream>
#include <iostream>

enum CameraModelID
{
    SIMPLE_PINHOLE = 0,
    PINHOLE = 1
};

struct CameraModelInfo
{
    std::string model_name;
    int num_params;
};

static const std::map<int, CameraModelInfo> CAMERA_MODEL_IDS = {
    {SIMPLE_PINHOLE, {"SIMPLE_PINHOLE", 3}},
    {PINHOLE, {"PINHOLE", 4}}};

CameraDataLoader::IntrinsicParams::IntrinsicParams() : model_id(0), width(0), height(0) {}
CameraDataLoader::IntrinsicParams::IntrinsicParams(const IntrinsicParams &other)
    : model_id(other.model_id), model_name(other.model_name),
      width(other.width), height(other.height), params(other.params) {}
CameraDataLoader::IntrinsicParams &CameraDataLoader::IntrinsicParams::operator=(const IntrinsicParams &other)
{
    if (this != &other)
    {
        model_id = other.model_id;
        model_name = other.model_name;
        width = other.width;
        height = other.height;
        params = other.params;
    }
    return *this;
}

CameraDataLoader::CameraDataLoader(const std::string &projectPath)
    : m_projectPath(projectPath) {}

bool CameraDataLoader::loadCameraData()
{
    std::string cameraIntrinsicPath = m_projectPath + "/sparse/0/cameras.bin";
    std::string cameraExtrinsicPath = m_projectPath + "/sparse/0/images.bin";

    ReadIntrinsicBinary(cameraIntrinsicPath.c_str());
    ReadExtrinsicBinary(cameraExtrinsicPath.c_str());

    return true;
}

std::vector<CameraInfo> &CameraDataLoader::getCameras()
{
    return m_cameras;
}

bool CameraDataLoader::loadCameraImage(size_t index)
{
    if (index >= m_cameras.size())
        return false;
    return m_cameras[index].loadImage();
}

void CameraDataLoader::loadAllImages()
{
    for (auto &cam : m_cameras)
    {
        cam.loadImage();
    }
}

void CameraDataLoader::ReadIntrinsicBinary(const char *filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    uint64_t num_cameras;
    file.read(reinterpret_cast<char *>(&num_cameras), sizeof(uint64_t));

    for (uint64_t i = 0; i < num_cameras; i++)
    {
        IntrinsicParams params;
        uint32_t camera_id;

        file.read(reinterpret_cast<char *>(&camera_id), sizeof(uint32_t));
        file.read(reinterpret_cast<char *>(&params.model_id), sizeof(int32_t));
        file.read(reinterpret_cast<char *>(&params.width), sizeof(uint64_t));
        file.read(reinterpret_cast<char *>(&params.height), sizeof(uint64_t));

        auto model_info = CAMERA_MODEL_IDS.find(params.model_id);
        if (model_info == CAMERA_MODEL_IDS.end())
        {
            std::cerr << "Invalid camera model ID: " << params.model_id << std::endl;
            exit(EXIT_FAILURE);
        }

        params.model_name = model_info->second.model_name;
        int num_params = model_info->second.num_params;
        params.params.resize(num_params);
        file.read(reinterpret_cast<char *>(params.params.data()), sizeof(double) * num_params);

        m_intrinsics[camera_id] = params;
    }

    file.close();
}

void CameraDataLoader::ReadExtrinsicBinary(const char *filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    uint64_t num_reg_images;
    file.read(reinterpret_cast<char *>(&num_reg_images), sizeof(uint64_t));

    for (uint64_t i = 0; i < num_reg_images; i++)
    {
        uint32_t image_id;
        std::vector<double> qvec(4);
        std::vector<double> tvec(3);
        uint32_t camera_id;
        std::string image_name;

        file.read(reinterpret_cast<char *>(&image_id), sizeof(uint32_t));
        file.read(reinterpret_cast<char *>(qvec.data()), sizeof(double) * 4);
        file.read(reinterpret_cast<char *>(tvec.data()), sizeof(double) * 3);
        file.read(reinterpret_cast<char *>(&camera_id), sizeof(uint32_t));

        char c;
        while (file.read(&c, 1) && c != '\0')
        {
            image_name += c;
        }

        uint64_t num_points2D;
        file.read(reinterpret_cast<char *>(&num_points2D), sizeof(uint64_t));
        file.seekg((std::streamoff)(num_points2D * (2 * sizeof(double) + sizeof(uint64_t))), std::ios::cur);

        auto &intr = m_intrinsics[camera_id];
        mat3 R = utils::qvec2rotmat(utils::vector_to_vec4(qvec));
        vec3 t = utils::vector_to_vec3(tvec);
        mat4 pose(
            R[0][0], R[0][1], R[0][2], t[0],
            R[1][0], R[1][1], R[1][2], t[1],
            R[2][0], R[2][1], R[2][2], t[2],
            0, 0, 0, 1);

        float fovx, fovy;
        switch (intr.model_id)
        {
        case SIMPLE_PINHOLE:
            fovx = utils::focal2fov(intr.params[0], intr.width);
            fovy = utils::focal2fov(intr.params[0], intr.height);
            break;
        case PINHOLE:
            fovx = utils::focal2fov(intr.params[0], intr.width);
            fovy = utils::focal2fov(intr.params[1], intr.height);
            break;
        default:
            std::cerr << "Unsupported camera model: " << intr.model_id << std::endl;
            continue;
        }

        std::string img_path = m_projectPath + "/images/" + image_name;
        m_cameras.emplace_back(image_id, pose, t, fovy, fovx,
                               img_path, image_name,
                               intr.width, intr.height);
    }

    file.close();
}
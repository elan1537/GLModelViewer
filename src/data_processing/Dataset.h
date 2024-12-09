#ifndef DATASET_H
#define DATASET_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <Angel.h>
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum CameraModelId
{
    SIMPLE_PINHOLE = 0,
    PINHOLE = 1,
    SIMPLE_RADIAL = 2,
    RADIAL = 3,
    OPENCV = 4,
    OPENCV_FISHEYE = 5,
    FULL_OPENCV = 6,
    FOV = 7,
    SIMPLE_RADIAL_FISHEYE = 8,
    RADIAL_FISHEYE = 9,
    THIN_PRISM_FISHEYE = 10
};

struct CameraModelInfo
{
    std::string model_name;
    int num_params;
};

const std::map<int, CameraModelInfo> CAMERA_MODEL_IDS = {
    {SIMPLE_PINHOLE, {"SIMPLE_PINHOLE", 3}},
    {PINHOLE, {"PINHOLE", 4}},
    {SIMPLE_RADIAL, {"SIMPLE_RADIAL", 4}},
    {RADIAL, {"RADIAL", 5}},
    {OPENCV, {"OPENCV", 8}},
    {OPENCV_FISHEYE, {"OPENCV_FISHEYE", 8}},
    {FULL_OPENCV, {"FULL_OPENCV", 12}},
    {FOV, {"FOV", 5}},
    {SIMPLE_RADIAL_FISHEYE, {"SIMPLE_RADIAL_FISHEYE", 4}},
    {RADIAL_FISHEYE, {"RADIAL_FISHEYE", 5}},
    {THIN_PRISM_FISHEYE, {"THIN_PRISM_FISHEYE", 12}}};

class CameraInfo
{
public:
    uint32_t uid;
    mat4 R; // Rotation matrix
    vec3 T; // Translation vector
    float FovY;
    float FovX;
    std::string image_path;
    std::string image_name;
    uint64_t width;
    uint64_t height;

    // 이미지 데이터 (필요할 때만 로드)
    unsigned char *image_data = nullptr;
    int image_width = 0;
    int image_height = 0;
    int image_channels = 0;
    GLuint textureID = 0;

    CameraInfo(uint32_t _uid, const mat4 &_R, const vec3 &_T,
               float _FovY, float _FovX,
               const std::string &_image_path, const std::string &_image_name,
               uint64_t _width, uint64_t _height)
        : uid(_uid), R(_R), T(_T), FovY(_FovY), FovX(_FovX),
          image_path(_image_path), image_name(_image_name),
          width(_width), height(_height) {}

    // 이미지 로드 (필요할 때 호출)
    bool loadImage()
    {
        if (image_data)
            return true; // 이미 로드된 경우
    
        stbi_set_flip_vertically_on_load(false);
        image_data = stbi_load(image_path.c_str(),
                               &image_width, &image_height, &image_channels, 0);

        if (!image_data)
        {
            std::cerr << "Failed to load image: " << image_path << std::endl;
            return false;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        // 텍스처 파라미터 설정
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // 픽셀 저장 방식 설정
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        
        // 텍스처 데이터 업로드
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, 
                    GL_RGB, GL_UNSIGNED_BYTE, image_data);

        // 이미지 데이터 해제
        stbi_image_free(image_data);
        image_data = nullptr;
        
        return true;
    }

    // 리소스 해제
    void unloadImage()
    {
        if (image_data)
        {
            stbi_image_free(image_data);
            image_data = nullptr;
        }
    }

    ~CameraInfo()
    {
        unloadImage();
    }
};

class Dataset
{
private:
    std::vector<CameraInfo> cameras;
    std::string projectPath;

    // 내부 파라미터 저장을 위한 구조체를 public으로 이동하고 복사 생성자 추가
    struct IntrinsicParams
    {
        int32_t model_id;
        std::string model_name;
        uint64_t width;
        uint64_t height;
        std::vector<double> params;

        // 기본 생성자
        IntrinsicParams() : model_id(0), width(0), height(0) {}

        // 복사 생성자
        IntrinsicParams(const IntrinsicParams &other)
            : model_id(other.model_id), model_name(other.model_name),
              width(other.width), height(other.height), params(other.params) {}

        // 할당 연산자
        IntrinsicParams &operator=(const IntrinsicParams &other)
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
    };

    std::map<int, IntrinsicParams> intrinsics;

public:
    Dataset(const char *filePath) : projectPath(filePath)
    {
        std::string imagePath = std::string(projectPath) + "/images";
        std::string cameraIntrinsicPath = std::string(projectPath) + "/sparse/0/cameras.bin";
        std::string cameraExtrinsicPath = std::string(projectPath) + "/sparse/0/images.bin";

        ReadIntrinsicBinary(cameraIntrinsicPath.c_str());
        ReadExtrinsicBinary(cameraExtrinsicPath.c_str());
    }

    const std::vector<CameraInfo> &getCameras() const { return cameras; }

    // 특정 카메라의 이미지 로드
    bool loadCameraImage(size_t index)
    {
        if (index >= cameras.size())
            return false;
        return cameras[index].loadImage();
    }

    // 모든 카메라의 이미지 로드
    void loadAllImages()
    {
        for (auto &cam : cameras)
        {
            cam.loadImage();
        }
    }

    void ReadIntrinsicBinary(const char *filePath)
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

            // Read camera properties (24 bytes)
            file.read(reinterpret_cast<char *>(&camera_id), sizeof(uint32_t));
            file.read(reinterpret_cast<char *>(&params.model_id), sizeof(int32_t));
            file.read(reinterpret_cast<char *>(&params.width), sizeof(uint64_t));
            file.read(reinterpret_cast<char *>(&params.height), sizeof(uint64_t));

            // Get model name and number of parameters
            auto model_info = CAMERA_MODEL_IDS.find(params.model_id);
            if (model_info == CAMERA_MODEL_IDS.end())
            {
                std::cerr << "Invalid camera model ID: " << params.model_id << std::endl;
                exit(EXIT_FAILURE);
            }

            params.model_name = model_info->second.model_name;
            int num_params = model_info->second.num_params;

            // Read camera parameters
            params.params.resize(num_params);
            file.read(reinterpret_cast<char *>(params.params.data()), sizeof(double) * num_params);

            // Store camera info
            intrinsics[camera_id] = params;
        }

        file.close();
    }

    void ReadExtrinsicBinary(const char *filePath)
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

            // 2D points 스킵 (현재는 사용하지 않음)
            uint64_t num_points2D;
            file.read(reinterpret_cast<char *>(&num_points2D), sizeof(uint64_t));
            file.seekg(num_points2D * (2 * sizeof(double) + sizeof(uint64_t)), std::ios::cur);

            // 카메라 정보 생성
            mat3 R = transpose(utils::qvec2rotmat(utils::vector_to_vec4(qvec)));
            vec3 t = utils::vector_to_vec3(tvec);
            mat4 pose = mat4(
                R[0][0], R[0][1], R[0][2], t[0],
                R[1][0], R[1][1], R[1][2], t[1],
                R[2][0], R[2][1], R[2][2], t[2],
                0, 0, 0, 1);

            auto &intr = intrinsics[camera_id];
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

            std::string image_path = std::string(projectPath) + "/images/" + image_name;

            cameras.emplace_back(image_id, pose, t, fovy, fovx,
                                 image_path, image_name,
                                 intr.width, intr.height);
        }

        file.close();
    }
};

#endif
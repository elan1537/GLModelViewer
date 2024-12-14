#pragma once

#include <string>
#include "utils/Angel.h"

class CameraInfo
{
public:
    CameraInfo(uint32_t uid, mat4 &R, vec3 &T, float FovY, float FovX, const std::string &image_path, const std::string &image_name, uint64_t width, uint64_t height);
    ~CameraInfo();

    bool loadImage();
    void unloadImage();

    uint32_t getUID() const;
    const mat4 &getPose() const;
    const vec3 &getTranslation() const;
    float getFovY() const;
    float getFovX() const;
    uint64_t getWidth() const;
    uint64_t getHeight() const;
    GLuint getTextureID() const;
    const std::string &getImagePath() const;
    const std::string &getImageName() const;

private:
    uint32_t m_uid;
    mat4 m_R;
    vec3 m_T;
    float m_FovY;
    float m_FovX;
    std::string m_imagePath;
    std::string m_imageName;
    uint64_t m_width;
    uint64_t m_height;

    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageChannels = 0;
    GLuint m_textureID = 0;
};
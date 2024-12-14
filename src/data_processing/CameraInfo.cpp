#include "CameraInfo.h"

#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"
#include <iostream>

CameraInfo::CameraInfo(uint32_t uid, mat4 &R, vec3 &T,
                       float FovY, float FovX,
                       const std::string &image_path, const std::string &image_name,
                       uint64_t width, uint64_t height)
    : m_uid(uid), m_R(R), m_T(T), m_FovY(FovY), m_FovX(FovX), m_imagePath(image_path), m_imageName(image_name), m_width(width), m_height(height) {}

CameraInfo::~CameraInfo()
{
    unloadImage();
}

bool CameraInfo::loadImage()
{
    if (m_textureID != 0)
        return true;

    stbi_set_flip_vertically_on_load(false);
    unsigned char *data = stbi_load(m_imagePath.c_str(), &m_imageWidth, &m_imageHeight, &m_imageChannels, 0);

    if (!data)
    {
        std::cerr << "Failed to load image: " << m_imagePath << std::endl;
        return false;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    if (m_imageChannels == 4)
        format = GL_RGBA;
    else if (m_imageChannels == 1)
        format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_imageWidth, m_imageHeight, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return true;
}

void CameraInfo::unloadImage()
{
    if (m_textureID != 0)
    {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
}

uint32_t CameraInfo::getUID() const { return m_uid; }
const mat4 &CameraInfo::getPose() const { return m_R; }
const vec3 &CameraInfo::getTranslation() const { return m_T; }
float CameraInfo::getFovY() const { return m_FovY; }
float CameraInfo::getFovX() const { return m_FovX; }
uint64_t CameraInfo::getWidth() const { return m_width; }
uint64_t CameraInfo::getHeight() const { return m_height; }
GLuint CameraInfo::getTextureID() const { return m_textureID; }
const std::string &CameraInfo::getImagePath() const { return m_imagePath; }
const std::string &CameraInfo::getImageName() const { return m_imageName; }
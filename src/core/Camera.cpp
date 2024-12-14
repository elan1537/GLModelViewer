#include "Camera.h"

Camera::Camera() : position(0.0f, 0.0f, 3.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f)
{
    pose = LookAt(position, target, up);
    std::cout << "pose: " << pose << std::endl;
}
Camera::~Camera() {}

void Camera::Update()
{
    // 카메라 업데이트 로직 구현
}
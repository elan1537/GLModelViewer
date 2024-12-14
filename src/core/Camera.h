#ifndef CAMERA_H
#define CAMERA_H

#include "utils/Angel.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void Update();

    vec3 position;
    vec3 target;
    vec3 up;

    mat4 pose;
};

#endif
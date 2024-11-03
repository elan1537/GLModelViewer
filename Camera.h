#ifndef CAMERA_H
#define CAMERA_H

#include "Angel.h"

class Camera {
public:
    vec3 position;    // 카메라 위치
    vec3 target;      // 카메라가 바라보는 대상 위치
    vec3 up;          // 카메라의 업 벡터
    mat4 viewMatrix;  // 뷰 행렬

    Camera(const vec3& position, const vec3& target, const vec3& up) {
        this->position = position;
        this->target = target;
        this->up = up;
        updateViewMatrix();
    }

    void updateViewMatrix() {
        viewMatrix = LookAt(position, target, up);
    }

    void move(const vec3& delta) {
        position += delta;
        target += delta;

        updateViewMatrix();
    }

    void rotate(float deltaX, float deltaY) {
        vec3 direction = position - target;
        mat4 rotation = RotateX(deltaY * DegreesToRadians) * RotateY(deltaX * DegreesToRadians);
        vec4 rotatedDirection = rotation * vec4(direction, 1.0f);
        position = vec3(rotatedDirection.x, rotatedDirection.y, rotatedDirection.z) + target;

        vec4 rotatedUp = rotation * vec4(up, 1.0f);
        up = vec3(rotatedUp.x, rotatedUp.y, rotatedUp.z);

        updateViewMatrix();
    }

    void zoom(float amount) {
        vec3 direction = normalize(target - position);
        position += direction * amount;

        updateViewMatrix();
    }

    void pan(float deltaX, float deltaY) {
        vec3 right = normalize(cross(target - position, up));
        vec3 delta = right * deltaX + up * deltaY;

        position += delta;
        target += delta;

        updateViewMatrix();
    }
};

#endif
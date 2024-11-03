#ifndef CAMERA_H
#define CAMERA_H

#include "Angel.h"

class Camera {
public:
    vec3 position;    // 카메라 위치
    vec3 target;      // 카메라가 바라보는 대상 위치
    vec3 up;          // 카메라의 업 벡터
    mat4 viewMatrix;  // 뷰 행렬

    // Ortho 최대, 최소 비율
    float minScale = 0.2f;
    float maxScale = 5.0f;

    // Perspective 최소 및 최대 거리 설정
    float minDistance = 0.5f;
    float maxDistance = 50.0f;

    bool isPerspective = true;
    float orthographicsScale = 1.0f;

    Camera(const vec3& position, const vec3& target, const vec3& up) {
        this->position = position;
        this->target = target;
        this->up = up;
        updateViewMatrix();
    }

    vec3 getFront() const {
        /* 
        카메라의 앞 방향을 향하는 Vector 반환
        */
        return normalize(target - position);
    }

    vec3 getRight() const {
        /*
        카메라의 오른쪽 방향을 향하는 Vector 반환
        */
        return normalize(cross(getFront(), up));
    }

    vec3 getUp() const {
        /*
        카메라의 위 방향을 향하는 Vector 반환
        */
        return normalize(cross(getRight(), getFront()));
    }

    void updateViewMatrix() {
        /* 카메라 view 행렬 업데이트 */
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
        if (isPerspective) { // Perspective 투영일 때
            vec3 direction = normalize(target - position);

            // 현재 카메라와 대상 사이의 거리 계산
            float currentDistance = length(position - target);

            // 원하는 새로운 거리 계산
            float desiredDistance = currentDistance - amount;

            // 새로운 거리가 최소 거리보다 작으면 이동하지 않음
            if (desiredDistance < minDistance) {
                desiredDistance = minDistance;
            }
            // 새로운 거리가 최대 거리보다 크면 이동하지 않음
            else if (desiredDistance > maxDistance) {
                desiredDistance = maxDistance;
            }

            // 카메라 위치 업데이트
            position = target - direction * desiredDistance;
            updateViewMatrix();
        } else { // Ortho 투영일 때
            orthographicsScale += amount;

            // minScale < orthographicsScale < maxScale 안으로 Zoom이 되게...
            if (orthographicsScale < minScale) {
                orthographicsScale = minScale;
            } else if (orthographicsScale > maxScale) {
                orthographicsScale = maxScale;
            }
        }
    }

    void pan(float deltaX, float deltaY) {
        vec3 delta = getRight() * deltaX + up * deltaY;

        position += delta;
        target += delta;

        updateViewMatrix();
    }
};

#endif
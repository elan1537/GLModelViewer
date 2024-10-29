#include "system.h"

// system 클래스의 lookAt 함수 구현
mat4 system::lookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 zAxis = normalize(eye - center);
    vec3 xAxis = normalize(cross(up, zAxis));
    vec3 yAxis = cross(zAxis, xAxis);

    mat4 rotation = mat4(
        vec4(xAxis, 0.0f),
        vec4(yAxis, 0.0f),
        vec4(zAxis, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    mat4 translation = mat4(
        vec4(1.0f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 1.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(-eye, 1.0f)
    );

    return rotation * translation;
}

// system 클래스의 perspective 함수 구현
mat4 system::perspective(float fovy, float aspect, float zNear, float zFar) {
    float tanHalfFovy = tan(fovy / 2.0f);

    mat4 result = mat4(0.0f);
    result[0][0] = 1.0f / (aspect * tanHalfFovy);
    result[1][1] = 1.0f / (tanHalfFovy);
    result[2][2] = -(zFar + zNear) / (zFar - zNear);
    result[2][3] = -1.0f;
    result[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);

    return result;
}

mat4 system::ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
    return mat4(
        2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
        0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
        0.0f, 0.0f, -2.0f / (zFar - zNear), -(zFar + zNear) / (zFar - zNear),
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

float system::toRadian(float degree) {
    return degree * M_PI / 180.0f;
}

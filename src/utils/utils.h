#pragma once
#include "Angel.h"
#include <vector>

namespace utils
{
    float getZFromXY(vec2 xy);
    mat3 qvec2rotmat(const vec4 &qvec);
    vec4 rotmat2qvec(const mat3 &R);
    vec3 vector_to_vec3(const std::vector<double> &v);
    vec4 vector_to_vec4(const std::vector<double> &v);
    double fov2focal(double fov, double pixels);
    double focal2fov(double focal, double pixels);
    mat3 RotateMatrix(const vec3 &axis, float angle);
}
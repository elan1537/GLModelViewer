#ifndef QUAT_H
#define QUAT_H

#include "Angel.h"
#include <iostream>

using namespace std;

class Quaternion {
    public:
        float x, y, z, w;
        Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        Quaternion operator+(const Quaternion& q) {
            return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
        }

        Quaternion operator-(const Quaternion& q) {
            return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
        }

        Quaternion operator*(const Quaternion& q) {
            return Quaternion(
                w * q.x + x * q.w + y * q.z - z * q.y,
                w * q.y + y * q.w + z * q.x - x * q.z,
                w * q.z + z * q.w + x * q.y - y * q.x,
                w * q.w - x * q.x - y * q.y - z * q.z
            );
        }

        Quaternion normalize() const {
            float magnitude = sqrt(x * x + y * y + z * z + w * w);
            return Quaternion(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
        }

        Quaternion conjugate() const {
            return Quaternion(-x, -y, -z, w);
        }

        ostream& operator<<(ostream& os) {
            os << "x: " << x << " y: " << y << " z: " << z << " w: " << w;
            return os;
        }

        static mat4 quaternionToMatrix(const Quaternion &q) {
            float xx = q.x * q.x;
            float yy = q.y * q.y;
            float zz = q.z * q.z;
            float xy = q.x * q.y;
            float xz = q.x * q.z;
            float yz = q.y * q.z;
            float wx = q.w * q.x;
            float wy = q.w * q.y;
            float wz = q.w * q.z;

            return mat4(
                1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),          2.0f * (xz + wy),           0.0f,
                2.0f * (yz + wz),        1.0f - 2.0f * (xx + zz),   2.0f * (yz - wx),           0.0f,
                2.0f * (xz - wy),        2.0f * (yz + wx),          1.0f - 2.0f * (xx + yy),    0.0f,
                0.0f,                    0.0f,                      0.0f,                       1.0f
            );
        };
};

#endif
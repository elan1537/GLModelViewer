#include "Angel.h"

class system {
    public:
        static mat4 lookAt(vec3 eye, vec3 center, vec3 up);
        static mat4 perspective(float fovy, float aspect, float zNear, float zFar);
        static float toRadian(float degree);
};
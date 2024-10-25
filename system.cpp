#include "system.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// system 클래스의 lookAt 함수 구현
glm::mat4 system::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    return glm::lookAt(eye, center, up);
}

// system 클래스의 perspective 함수 구현
glm::mat4 system::perspective(float fovy, float aspect, float zNear, float zFar) {
    return glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class system {
    public:
        static glm::mat4 lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
        static glm::mat4 perspective(float fovy, float aspect, float zNear, float zFar);
};
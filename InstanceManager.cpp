#include "InstanceManager.h"

InstanceManager::InstanceManager(GLuint shaderProgram)
    : shaderProgram(shaderProgram) {}

void InstanceManager::addObject(Object* obj) {
    objects.push_back(obj);
}

void InstanceManager::renderAll() {
    /* objects 배열에 관리되는 모든 객체의 model 행렬과 vertices를 전달함 */
    for (auto object : objects) {
        glBindVertexArray(object->getVAO());

        mat4 model = object->getModelMatrix();

        // Model Matrix 전송
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "u_model");
        glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &model[0][0]);

        // Draw call
        glDrawElements(GL_TRIANGLES, object->getFaceCount() * 3, GL_UNSIGNED_INT, 0);
    }
}
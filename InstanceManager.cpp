#include "InstanceManager.h"

InstanceManager::InstanceManager(GLuint shaderProgram)
    : shaderProgram(shaderProgram) {}

void InstanceManager::addObject(Object* obj) {
    objects.push_back(obj);
}

void InstanceManager::renderAll(mat4& viewMatrix, mat4& projectionMatrix) {
    for (auto object : objects) {
        glBindVertexArray(object->getVAO());

        // Model Matrix 전송
        mat4 model = object->getModelMatrix();
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "u_model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        // View와 Projection 매트릭스를 쉐이더로 전송
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);

        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

        // Draw call
        glDrawElements(GL_TRIANGLES, object->getFaceCount() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Unbind VAO
    }
}
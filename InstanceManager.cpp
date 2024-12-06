#include "InstanceManager.h"

InstanceManager::InstanceManager(Shader &shader)
    : shader(shader) {}

void InstanceManager::addObject(Object *obj)
{
    objects.push_back(obj);
}

void InstanceManager::renderAll()
{
    /* objects 배열에 관리되는 모든 객체의 model 행렬과 vertices를 전달함 */
    for (auto object : objects)
    {
        glBindVertexArray(object->getVAO());
        mat4 model = object->getModelMatrix();

        // Model Matrix 전송
        shader.setMat4("u_model", model, GL_TRUE);

        // Draw call
        // glDrawElements(GL_TRIANGLES, object->getFaceCount() * 3, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_POINTS, 0, object->getVertexCount());
    }
}
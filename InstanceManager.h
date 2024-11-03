#ifndef INSTANCEMANAGER_H
#define INSTANCEMANAGER_H

#include <vector>
#include "Object.h"
#include <GL/glew.h>
#include "Angel.h"

class InstanceManager {
public:
    std::vector<Object*> objects;
    InstanceManager(GLuint shaderProgram);

    void addObject(Object* obj);

    void renderAll(mat4& viewMatrix, mat4& projectionMatrix);

private:
    GLuint shaderProgram;
};

#endif // INSTANCEMANAGER_H
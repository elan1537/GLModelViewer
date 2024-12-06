#ifndef INSTANCEMANAGER_H
#define INSTANCEMANAGER_H

#include <vector>
#include "Shader.h"
#include "Object.h"
#include <GL/glew.h>
#include "Angel.h"

class InstanceManager
{
public:
    std::vector<Object *> objects;
    InstanceManager(Shader &shader);

    void addObject(Object *obj);

    void renderAll();

private:
    Shader &shader;
};

#endif // INSTANCEMANAGER_H
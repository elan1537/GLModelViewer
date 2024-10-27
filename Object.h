#ifndef OBJECT_H
#define OBJECT_H
#include <GL/glew.h>
#include "Angel.h"
#include "MeshReader.h"

class Object {
public:
    Object(MeshReader* meshReader, const vec3& position, const vec3& rotation, const vec3& scale);
    ~Object();
    
    mat4 getModelMatrix() const;
    int getFaceCount() const;
    GLuint getVAO() const;

private:
    MeshReader* mesh;
    vec3 position, rotation, scale;
    GLuint VAO, VBO, EBO;
};
#endif
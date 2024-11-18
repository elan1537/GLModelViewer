#include "Object.h"

Object::Object(MeshReader *meshReader, const vec3 &position, const vec3 &rotation, const vec3 &scale)
{
    this->mesh = meshReader;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->getVertices().size() * sizeof(Vertex), &mesh->getVertices()[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaces().size() * sizeof(Face), &mesh->getFaces()[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Object::~Object()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

mat4 Object::getModelMatrix() const
{
    mat4 model = Scale(scale);
    model = RotateZ(rotation.z) * RotateY(rotation.y) * RotateX(rotation.x) * model;
    model = Translate(position) * model;
    return model;
}

int Object::getFaceCount() const
{
    return this->mesh->getFaces().size();
}

GLuint Object::getVAO() const
{
    return VAO;
}
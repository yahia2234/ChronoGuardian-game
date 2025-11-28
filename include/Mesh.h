#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void draw() const;

    // Static helper functions to create common shapes
    static Mesh* createCube(float size = 1.0f);
    static Mesh* createSphere(float radius = 1.0f, int sectors = 36, int stacks = 18);
    static Mesh* createPlane(float width = 1.0f, float height = 1.0f);
    static Mesh* createCylinder(float radius = 1.0f, float height = 2.0f, int sectors = 36);
    static Mesh* createCone(float radius = 1.0f, float height = 2.0f, int sectors = 36);

private:
    void setupMesh();
};

#endif

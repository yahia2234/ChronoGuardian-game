#include "Mesh.h"
#include <cmath>

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& inds)
    : vertices(verts), indices(inds) {
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // TexCoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh* Mesh::createCube(float size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float s = size / 2.0f;

    // Define 24 vertices (4 per face for proper normals)
    glm::vec3 positions[24] = {
        // Front face
        {-s, -s,  s}, { s, -s,  s}, { s,  s,  s}, {-s,  s,  s},
        // Back face
        { s, -s, -s}, {-s, -s, -s}, {-s,  s, -s}, { s,  s, -s},
        // Top face
        {-s,  s,  s}, { s,  s,  s}, { s,  s, -s}, {-s,  s, -s},
        // Bottom face
        {-s, -s, -s}, { s, -s, -s}, { s, -s,  s}, {-s, -s,  s},
        // Right face
        { s, -s,  s}, { s, -s, -s}, { s,  s, -s}, { s,  s,  s},
        // Left face
        {-s, -s, -s}, {-s, -s,  s}, {-s,  s,  s}, {-s,  s, -s}
    };

    glm::vec3 normals[6] = {
        { 0,  0,  1}, { 0,  0, -1}, { 0,  1,  0},
        { 0, -1,  0}, { 1,  0,  0}, {-1,  0,  0}
    };

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            Vertex v;
            v.position = positions[i * 4 + j];
            v.normal = normals[i];
            v.texCoord = glm::vec2((j == 1 || j == 2) ? 1.0f : 0.0f, (j == 2 || j == 3) ? 1.0f : 0.0f);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < 6; i++) {
        int base = i * 4;
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    return new Mesh(vertices, indices);
}

Mesh* Mesh::createSphere(float radius, int sectors, int stacks) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;

            Vertex v;
            v.position.x = xy * cosf(sectorAngle);
            v.position.y = z;
            v.position.z = xy * sinf(sectorAngle);
            v.normal = glm::normalize(v.position);
            v.texCoord.x = (float)j / sectors;
            v.texCoord.y = (float)i / stacks;
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return new Mesh(vertices, indices);
}

Mesh* Mesh::createPlane(float width, float height) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float w = width / 2.0f;
    float h = height / 2.0f;

    vertices.push_back({{-w, 0, -h}, {0, 1, 0}, {0, 0}});
    vertices.push_back({{ w, 0, -h}, {0, 1, 0}, {1, 0}});
    vertices.push_back({{ w, 0,  h}, {0, 1, 0}, {1, 1}});
    vertices.push_back({{-w, 0,  h}, {0, 1, 0}, {0, 1}});

    indices = {0, 1, 2, 0, 2, 3};

    return new Mesh(vertices, indices);
}

Mesh* Mesh::createCylinder(float radius, float height, int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * M_PI / sectors;
    float halfHeight = height / 2.0f;

    // Side vertices
    for (int i = 0; i <= sectors; ++i) {
        float sectorAngle = i * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);

        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        vertices.push_back({{x, -halfHeight, z}, normal, {(float)i / sectors, 0}});
        vertices.push_back({{x,  halfHeight, z}, normal, {(float)i / sectors, 1}});
    }

    // Side indices
    for (int i = 0; i < sectors; ++i) {
        int k1 = i * 2;
        int k2 = k1 + 2;

        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);

        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
    }

    return new Mesh(vertices, indices);
}

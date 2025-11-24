#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float size;
    float lifetime;
    float age;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f), size(1.0f), lifetime(1.0f), age(0.0f) {}
};

class ParticleSystem {
public:
    ParticleSystem(int maxParticles = 1000);
    ~ParticleSystem();

    void emit(const glm::vec3& position, const glm::vec3& velocity, const glm::vec4& color, 
              float size, float lifetime, int count = 1);
    void update(float deltaTime);
    void draw(const glm::mat4& view, const glm::mat4& projection);

    void clear();
    void emitExplosion(const glm::vec3& position, const glm::vec3& color, int count);

private:
    std::vector<Particle> particles;
    int maxParticles;
    GLuint VAO, VBO;

    void setupBuffers();
    void updateBuffers();
};

#endif

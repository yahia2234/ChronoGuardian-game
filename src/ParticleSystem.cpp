#include "ParticleSystem.h"
#include <algorithm>

ParticleSystem::ParticleSystem(int max) : maxParticles(max) {
    particles.reserve(maxParticles);
    setupBuffers();
}

ParticleSystem::~ParticleSystem() {
    glDeleteBuffers(1, &VBO);
}

void ParticleSystem::setupBuffers() {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::emit(const glm::vec3& position, const glm::vec3& velocity, 
                          const glm::vec4& color, float size, float lifetime, int count) {
    for (int i = 0; i < count && particles.size() < maxParticles; i++) {
        Particle p;
        p.position = position;
        p.velocity = velocity + glm::vec3(
            (rand() % 100 - 50) / 100.0f,
            (rand() % 100 - 50) / 100.0f,
            (rand() % 100 - 50) / 100.0f
        ) * 0.2f; // Slight random spread
        p.color = color;
        p.size = size;
        p.lifetime = lifetime;
        p.age = 0.0f;
        
        particles.push_back(p);
    }
}

void ParticleSystem::emitExplosion(const glm::vec3& position, const glm::vec3& color, int count) {
    for (int i = 0; i < count; i++) {
        // Random velocity in sphere
        float theta = ((rand() % 100) / 100.0f) * 2.0f * 3.14159f;
        float phi = ((rand() % 100) / 100.0f) * 3.14159f;
        float speed = 2.0f + ((rand() % 100) / 100.0f) * 3.0f;
        
        glm::vec3 vel(
            sin(phi) * cos(theta),
            cos(phi),
            sin(phi) * sin(theta)
        );
        
        // Call emit with correct arguments
        emit(position, vel * speed, glm::vec4(color, 1.0f), 0.2f, 1.0f, 1);
    }
}

void ParticleSystem::update(float deltaTime) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->age += deltaTime;
        
        if (it->age >= it->lifetime) {
            it = particles.erase(it);
        } else {
            it->position += it->velocity * deltaTime;
            it->velocity.y -= 9.8f * deltaTime; // Gravity
            
            // Fade out
            float lifeRatio = it->age / it->lifetime;
            it->color.a = 1.0f - lifeRatio;
            
            ++it;
        }
    }
}

void ParticleSystem::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (particles.empty()) return;

    updateBuffers();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Bind buffer and set up attributes (no VAO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Position attribute (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    
    // Color attribute (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    
    // Size attribute (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    
    glDrawArrays(GL_POINTS, 0, particles.size());
    
    // Cleanup
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
}

void ParticleSystem::updateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::clear() {
    particles.clear();
}

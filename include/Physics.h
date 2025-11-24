#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    bool intersects(const AABB& other) const;
    glm::vec3 getCenter() const { return (min + max) * 0.5f; }
    glm::vec3 getSize() const { return max - min; }
};

struct Sphere {
    glm::vec3 center;
    float radius;

    Sphere() : center(0.0f), radius(0.0f) {}
    Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}

    bool intersects(const Sphere& other) const;
    bool intersects(const AABB& box) const;
};

class Physics {
public:
    static bool checkAABBCollision(const AABB& a, const AABB& b);
    static bool checkSphereCollision(const Sphere& a, const Sphere& b);
    static bool checkSphereAABBCollision(const Sphere& sphere, const AABB& box);
    
    static glm::vec3 resolveAABBCollision(const AABB& moving, const AABB& stationary, const glm::vec3& velocity);
    static glm::vec3 resolveSphereCollision(const Sphere& a, const Sphere& b);
    
    static AABB createAABBFromTransform(const glm::vec3& position, const glm::vec3& size);
    static Sphere createSphereFromTransform(const glm::vec3& position, float radius);
    
    // Raycasting
    static bool rayIntersectAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const AABB& box, float& t);
};

#endif

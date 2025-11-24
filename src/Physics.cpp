#include "Physics.h"
#include <algorithm>

bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}

bool Sphere::intersects(const Sphere& other) const {
    float distance = glm::length(center - other.center);
    return distance < (radius + other.radius);
}

bool Sphere::intersects(const AABB& box) const {
    glm::vec3 closestPoint = glm::clamp(center, box.min, box.max);
    float distance = glm::length(closestPoint - center);
    return distance < radius;
}

bool Physics::checkAABBCollision(const AABB& a, const AABB& b) {
    return a.intersects(b);
}

bool Physics::checkSphereCollision(const Sphere& a, const Sphere& b) {
    return a.intersects(b);
}

bool Physics::checkSphereAABBCollision(const Sphere& sphere, const AABB& box) {
    return sphere.intersects(box);
}

glm::vec3 Physics::resolveAABBCollision(const AABB& moving, const AABB& stationary, const glm::vec3& velocity) {
    if (!moving.intersects(stationary)) {
        return velocity;
    }

    glm::vec3 penetration;
    penetration.x = std::min(moving.max.x - stationary.min.x, stationary.max.x - moving.min.x);
    penetration.y = std::min(moving.max.y - stationary.min.y, stationary.max.y - moving.min.y);
    penetration.z = std::min(moving.max.z - stationary.min.z, stationary.max.z - moving.min.z);

    glm::vec3 resolved = velocity;

    // Find the axis with minimum penetration
    if (penetration.x < penetration.y && penetration.x < penetration.z) {
        resolved.x = 0;
    } else if (penetration.y < penetration.z) {
        resolved.y = 0;
    } else {
        resolved.z = 0;
    }

    return resolved;
}

glm::vec3 Physics::resolveSphereCollision(const Sphere& a, const Sphere& b) {
    glm::vec3 direction = a.center - b.center;
    float distance = glm::length(direction);
    
    if (distance < a.radius + b.radius && distance > 0) {
        direction = glm::normalize(direction);
        float overlap = (a.radius + b.radius) - distance;
        return direction * overlap;
    }
    
    return glm::vec3(0.0f);
}

AABB Physics::createAABBFromTransform(const glm::vec3& position, const glm::vec3& size) {
    glm::vec3 halfSize = size * 0.5f;
    return AABB(position - halfSize, position + halfSize);
}

Sphere Physics::createSphereFromTransform(const glm::vec3& position, float radius) {
    return Sphere(position, radius);
}

bool Physics::rayIntersectAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const AABB& box, float& t) {
    // Slab method
    glm::vec3 dirInv = 1.0f / rayDir;
    
    float t1 = (box.min.x - rayOrigin.x) * dirInv.x;
    float t2 = (box.max.x - rayOrigin.x) * dirInv.x;
    float t3 = (box.min.y - rayOrigin.y) * dirInv.y;
    float t4 = (box.max.y - rayOrigin.y) * dirInv.y;
    float t5 = (box.min.z - rayOrigin.z) * dirInv.z;
    float t6 = (box.max.z - rayOrigin.z) * dirInv.z;
    
    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
    
    // If tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
    if (tmax < 0) {
        t = tmax;
        return false;
    }
    
    // If tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) {
        t = tmax;
        return false;
    }
    
    t = tmin;
    return true;
}

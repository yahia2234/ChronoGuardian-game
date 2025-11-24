#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    Transform() 
        : position(0.0f), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), scale(1.0f) {}

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);
        return model;
    }

    void translate(const glm::vec3& offset) {
        position += offset;
    }

    void rotate(float angle, const glm::vec3& axis) {
        rotation = glm::rotate(rotation, angle, axis);
    }

    void setRotation(const glm::vec3& eulerAngles) {
        rotation = glm::quat(eulerAngles);
    }

    glm::vec3 forward() const {
        return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 right() const {
        return glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 up() const {
        return glm::normalize(rotation * glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

#endif

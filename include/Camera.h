#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMode {
    FIRST_PERSON,
    THIRD_PERSON
};

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float fov;

    CameraMode mode;
    glm::vec3 targetPosition; // For third-person mode

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void updateCameraVectors();
    
    void setFirstPersonMode(const glm::vec3& playerPos);
    void setThirdPersonMode(const glm::vec3& playerPos);
    void toggleMode();

private:
    void updateVectors();
};

#endif

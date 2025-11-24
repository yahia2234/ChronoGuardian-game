#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), fov(45.0f), mode(THIRD_PERSON) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = -30.0f; // Good default angle
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    float sensitivity = 0.15f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch to prevent flipping
    if (constrainPitch) {
        if (mode == THIRD_PERSON) {
            // Limit vertical angle in 3rd person (don't go under ground or too high)
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -10.0f) pitch = -10.0f; // Don't look up from below ground
        } else {
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
    }

    // In First Person, we update vectors immediately
    if (mode == FIRST_PERSON) {
        updateVectors();
    }
    // In Third Person, vectors are updated in setThirdPersonMode based on player pos
}

void Camera::updateVectors() {
    // Only used for First Person or free cam
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::setFirstPersonMode(const glm::vec3& playerPos) {
    mode = FIRST_PERSON;
    position = playerPos;
    updateVectors(); // Use standard FPS look behavior
}

void Camera::setThirdPersonMode(const glm::vec3& playerPos) {
    mode = THIRD_PERSON;
    targetPosition = playerPos;
    
    // Orbit parameters
    float distance = 8.0f; // Closer for "slightly above" feel
    
    // Calculate camera position using Spherical Coordinates
    // Yaw rotates around Y axis, Pitch rotates up/down
    float camX = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    float camY = distance * sin(glm::radians(pitch)); // Height relative to player
    float camZ = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    
    // Position relative to player
    // Note: We subtract because we want the camera *behind* the direction of view
    // But for orbit, we just calculate the offset directly
    
    // Actually, let's use standard orbit math:
    // x = r * cos(pitch) * cos(yaw)
    // y = r * sin(pitch)
    // z = r * cos(pitch) * sin(yaw)
    
    // We want the camera to be at (yaw, pitch) on the sphere
    // And looking AT the center
    
    // Invert yaw for intuitive mouse control
    float theta = glm::radians(yaw - 90.0f); 
    float phi = glm::radians(pitch);
    
    glm::vec3 offset;
    offset.x = distance * cos(phi) * cos(theta);
    offset.y = distance * sin(phi); // This will be negative if pitch is negative
    offset.z = distance * cos(phi) * sin(theta);
    
    // If pitch is negative (looking down), camera should be ABOVE (positive Y offset)
    // So we negate the Y component of the offset calculation effectively
    // Let's simplify:
    // Pitch -30 means looking DOWN. Camera should be UP.
    // So camera Y should be +sin(-pitch) * distance
    
    float heightOffset = distance * sin(glm::radians(-pitch));
    float groundDist = distance * cos(glm::radians(-pitch));
    
    offset.x = groundDist * sin(glm::radians(yaw));
    offset.z = groundDist * cos(glm::radians(yaw));
    offset.y = heightOffset;
    
    // Add offset to player position
    // We want camera BEHIND player based on yaw
    // If yaw is 0, we are looking along -Z. Camera should be at +Z.
    
    position = playerPos + offset;
    
    // Look at player (slightly above center)
    glm::vec3 lookTarget = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
    front = glm::normalize(lookTarget - position);
    
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::toggleMode() {
    if (mode == FIRST_PERSON) {
        mode = THIRD_PERSON;
        pitch = -30.0f; // Reset to good angle
    } else {
        mode = FIRST_PERSON;
        pitch = 0.0f;
    }
}

void Camera::updateCameraVectors() {
    // Helper to force update if needed
    if (mode == FIRST_PERSON) updateVectors();
}


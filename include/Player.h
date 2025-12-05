#ifndef PLAYER_H
#define PLAYER_H

#include "Mesh.h"
#include "Model.h"
#include "ParticleSystem.h"
#include "Physics.h"
#include "Transform.h"
#include <memory>
#include <vector>

class Player {
public:
  Transform transform;
  Sphere collisionSphere;

  float moveSpeed;
  float hoverHeight;
  float bobSpeed;
  float bobAmount;

  glm::vec3 velocity;
  bool isFlashing;
  float flashTimer;

  // Orbiting fragments
  struct Fragment {
    Transform transform;
    float orbitRadius;
    float orbitSpeed;
    float orbitAngle;
    std::unique_ptr<Mesh> mesh;
  };

  std::vector<Fragment> fragments;
  std::unique_ptr<Mesh> coreMesh;
  std::unique_ptr<Mesh> headMesh;     // New head component
  std::unique_ptr<Model> playerModel; // For loading external models

  float fragmentRotationSpeed;
  float currentBobOffset;
  float walkBobOffset;  // Vertical offset for walking animation
  float cameraYaw;      // Track camera yaw for player model rotation
  bool controlsEnabled; // Can be disabled during cutscenes

  Player();
  ~Player();

  void update(float deltaTime, const glm::vec3 &moveInput);
  void draw(class Shader *shader);

  void onWallCollision(const glm::vec3 &normal, ParticleSystem *particles);
  void onObstacleHit(const glm::vec3 &knockbackDir, ParticleSystem *particles);

  glm::vec3 getPosition() const { return transform.position; }
  void setPosition(const glm::vec3 &pos) { transform.position = pos; }
  void setCameraYaw(float yaw) { cameraYaw = yaw; }

private:
  void updateFragments(float deltaTime, float movementMagnitude);
  void updateHoverAnimation(float deltaTime);
};

#endif

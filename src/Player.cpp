#include "Player.h"
#include "AudioManager.h"
#include "Input.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

Player::Player()
    : moveSpeed(8.0f), hoverHeight(1.0f), bobSpeed(2.0f), bobAmount(0.1f),
      velocity(0.0f), isFlashing(false), flashTimer(0.0f),
      fragmentRotationSpeed(1.0f), currentBobOffset(0.0f), walkBobOffset(0.0f),
      cameraYaw(0.0f), controlsEnabled(true) {

  // Create core mesh (ornate sphere - Ancient Gold)
  coreMesh.reset(Mesh::createSphere(0.5f, 36, 18));

  // Create head mesh (floating above core)
  headMesh.reset(Mesh::createSphere(0.25f, 16, 16));

  // Set initial position
  transform.position = glm::vec3(0.0f, hoverHeight, 0.0f);

  // Create collision sphere
  collisionSphere = Sphere(transform.position, 0.6f);

  // Create orbiting fragments (Ancient Stone)
  for (int i = 0; i < 6; i++) {
    Fragment frag;
    frag.orbitRadius = 1.2f;
    frag.orbitSpeed = 2.0f + (i * 0.2f);             // Varying speeds
    frag.orbitAngle = (i * 60.0f) * (M_PI / 180.0f); // Evenly spaced

    // Alternate between cubes and smaller spheres (Runes/Stones)
    if (i % 2 == 0) {
      frag.mesh.reset(Mesh::createCube(0.15f));
    } else {
      frag.mesh.reset(Mesh::createSphere(0.1f, 16, 8));
    }

    frag.transform.scale = glm::vec3(1.0f);
    fragments.push_back(std::move(frag));
  }

  // Load external model
  // Try to load the player model
  try {
    playerModel = std::make_unique<Model>("assets/models/player.glb");
  } catch (...) {
    std::cout << "Failed to load player model, falling back to procedural mesh."
              << std::endl;
  }
}

Player::~Player() {}

void Player::update(float deltaTime, const glm::vec3 &moveInput) {
  // Apply gravity
  const float gravity = -20.0f;
  const float groundLevel = 1.0f;

  // Apply gravity to velocity
  velocity.y += gravity * deltaTime;

  // Apply velocity to position
  transform.position += velocity * deltaTime;

  // Ground collision
  if (transform.position.y < groundLevel) {
    transform.position.y = groundLevel;
    velocity.y = 0.0f;
  }

  // Horizontal Movement (Smooth Gliding)
  glm::vec3 targetVelocity = glm::vec3(0.0f);
  if (controlsEnabled) {
    targetVelocity = glm::vec3(moveInput.x, 0.0f, moveInput.z) * moveSpeed;
  }

  // Acceleration/Deceleration parameters
  float acceleration = 15.0f;
  float friction = 10.0f;

  // Smoothly interpolate current velocity towards target velocity
  if (glm::length(moveInput) > 0.0f && controlsEnabled) {
    // Accelerate
    velocity.x =
        glm::mix(velocity.x, targetVelocity.x, acceleration * deltaTime);
    velocity.z =
        glm::mix(velocity.z, targetVelocity.z, acceleration * deltaTime);
  } else {
    // Decelerate (Friction)
    velocity.x = glm::mix(velocity.x, 0.0f, friction * deltaTime);
    velocity.z = glm::mix(velocity.z, 0.0f, friction * deltaTime);
  }

  // Apply horizontal velocity
  transform.position.x += velocity.x * deltaTime;
  transform.position.z += velocity.z * deltaTime;

  // JUMP MECHANICS
  // Check if grounded (vertical velocity is zero or player just landed)
  bool isGrounded =
      (std::abs(velocity.y) < 0.1f && transform.position.y >= groundLevel);

  // Jump input (SPACE key) - only if controls enabled
  if (controlsEnabled && Input::getInstance().isKeyJustPressed(KEY_SPACE) &&
      isGrounded) {
    velocity.y = 8.0f; // Jump force
  }

  // Update collision sphere
  collisionSphere.center = transform.position;

  // Update hover animation
  updateHoverAnimation(deltaTime);

  // Update fragments and walking bob
  float movementMagnitude = glm::length(glm::vec2(moveInput.x, moveInput.z));
  updateFragments(deltaTime, movementMagnitude);

  // Update walking bob animation
  if (movementMagnitude > 0.1f && isGrounded) {
    // Player is walking - animate bob
    walkBobOffset += deltaTime * 10.0f; // Speed of walking animation
    // Keep it in range to avoid overflow
    if (walkBobOffset > 2.0f * M_PI) {
      walkBobOffset -= 2.0f * M_PI;
    }
  } else {
    // Not walking - smoothly return to neutral position
    walkBobOffset *= 0.9f;
  }

  // Update flash effect
  if (isFlashing) {
    flashTimer -= deltaTime;
    if (flashTimer <= 0.0f) {
      isFlashing = false;
    }
  }

  // Movement sound
  static float movementSoundTimer = 0.0f;
  if (movementMagnitude > 0.1f) {
    movementSoundTimer += deltaTime;
    if (movementSoundTimer > 0.5f) {
      AudioManager::getInstance().playSound(SoundEffect::MOVEMENT, 0.3f);
      movementSoundTimer = 0.0f;
    }
  }
}

void Player::updateHoverAnimation(float deltaTime) {
  // Hover animation disabled - conflicts with gravity
  // Player stays at ground level (y=1.0)
  currentBobOffset += bobSpeed * deltaTime;
  // Don't modify position.y here - gravity handles it
}

void Player::updateFragments(float deltaTime, float movementMagnitude) {
  // Increase rotation speed when moving
  float speedMultiplier = 1.0f + (movementMagnitude * 2.0f);

  for (auto &frag : fragments) {
    // Update orbit angle
    frag.orbitAngle += frag.orbitSpeed * speedMultiplier * deltaTime;

    // Calculate position in orbit
    float x = cos(frag.orbitAngle) * frag.orbitRadius;
    float z = sin(frag.orbitAngle) * frag.orbitRadius;
    float y = sin(frag.orbitAngle * 2.0f) * 0.3f; // Vertical wobble

    frag.transform.position = transform.position + glm::vec3(x, y, z);

    // Rotate the fragment itself
    frag.transform.rotate(fragmentRotationSpeed * speedMultiplier * deltaTime,
                          glm::vec3(1.0f, 1.0f, 0.0f));
  }
}

void Player::draw(Shader *shader) {
  // Draw core (Ancient Gold)
  glm::vec3 coreColor =
      isFlashing ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.8f, 0.7f, 0.2f);

  shader->setMat4("model", transform.getModelMatrix());
  shader->setVec3("objectColor", coreColor);
  shader->setBool("useTexture", false);
  shader->setFloat("shininess", 64.0f);
  shader->setFloat("shininess", 64.0f);

  if (playerModel && !playerModel->meshes.empty()) {
    // Draw the imported model

    // 1. Disable culling temporarily (fixes inside-out models)
    glDisable(GL_CULL_FACE);

    glm::mat4 modelMatrix = transform.getModelMatrix();

    // Apply camera yaw rotation first (rotate the model to face camera
    // direction)
    modelMatrix =
        glm::rotate(modelMatrix, glm::radians(cameraYaw), glm::vec3(0, 1, 0));

    // Calculate walking bob (vertical offset)
    float bobY = sin(walkBobOffset) * 0.15f; // 0.15 units up/down amplitude

    // Center the model - adjust X, Y, Z offsets to align with player center
    // Add walking bob to Y offset
    modelMatrix =
        glm::translate(modelMatrix, glm::vec3(2.9f, -1.0f + bobY, -2.0f));

    // Scale - 0.1f for good size
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

    // Rotate 180° on Y-axis for correct facing direction (90° base + 90°
    // adjustment)
    modelMatrix =
        glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    // Fix upright orientation (X-axis rotation)
    modelMatrix =
        glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    shader->setMat4("model", modelMatrix);

    // 4. Set a bright default color (white) so it's visible even without
    // textures
    shader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));

    playerModel->draw(shader);

    // Re-enable culling
    glEnable(GL_CULL_FACE);
  } else {
    // Fallback to procedural mesh
    shader->setVec3("objectColor", coreColor);
    coreMesh->draw();
  }

  // Draw Head (Floating above) - Only if no model loaded
  if (!playerModel || playerModel->meshes.empty()) {
    glm::mat4 headModel = transform.getModelMatrix();
    headModel =
        glm::translate(headModel, glm::vec3(0.0f, 0.6f, 0.0f)); // Float above

    shader->setMat4("model", headModel);
    shader->setVec3("objectColor", coreColor); // Same gold color
    headMesh->draw();
  }

  // Draw fragments (Ancient Stone/Energy)
  glm::vec3 fragmentColor =
      glm::vec3(0.4f, 0.8f, 1.0f); // Glowing blue energy stones
  for (const auto &frag : fragments) {
    shader->setMat4("model", frag.transform.getModelMatrix());
    shader->setVec3("objectColor", fragmentColor);
    shader->setFloat("shininess", 32.0f);
    frag.mesh->draw();
  }
}

void Player::onWallCollision(const glm::vec3 &normal,
                             ParticleSystem *particles) {
  // Requirement: "Brief Translation in the opposite direction of impact"
  glm::vec3 bounceDir = normal;
  bounceDir.y = 0.0f; // Keep it horizontal
  if (glm::length(bounceDir) > 0.0f) {
    bounceDir = glm::normalize(bounceDir);
    transform.position += bounceDir * 0.5f; // Bounce back
  }

  // Emit spark particles
  if (particles) {
    particles->emit(transform.position + normal * 0.6f, normal * 3.0f,
                    glm::vec4(1.0f, 0.8f, 0.2f, 1.0f), 5.0f, 0.5f, 10);
  }

  // Play collision sound
  static float collisionCooldown = 0.0f;
  if (collisionCooldown <= 0.0f) {
    AudioManager::getInstance().playSound(SoundEffect::WALL_COLLISION, 0.6f);
    collisionCooldown = 0.2f;
  }
}

void Player::onObstacleHit(const glm::vec3 &knockbackDir,
                           ParticleSystem *particles) {
  // Strong knockback - increased for better effect
  transform.position += knockbackDir * 5.0f;

  // Flash red
  isFlashing = true;
  flashTimer = 0.3f;

  // Emit impact particles
  if (particles) {
    particles->emit(transform.position, knockbackDir * 8.0f,
                    glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), 8.0f, 0.7f, 20);
  }

  // Play obstacle hit sound
  AudioManager::getInstance().playSound(SoundEffect::OBSTACLE_HIT, 0.8f);
}

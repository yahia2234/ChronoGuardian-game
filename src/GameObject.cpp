#include "GameObject.h"
#include "AudioManager.h"
#include "Shader.h"
#include <cmath>

GameObject::GameObject(GameObjectType t)
    : type(t), color(1.0f), transparency(1.0f), materialType(0),
      texture(nullptr), isActive(true), isTrigger(false),
      useSphereCollision(false) {}

void GameObject::draw(Shader *shader) {
  if (!isActive || !mesh)
    return;

  glm::mat4 model = transform.getModelMatrix();
  shader->setMat4("model", model);

  // Calculate normal matrix for correct lighting with non-uniform scaling
  glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
  shader->setMat3("normalMatrix", normalMatrix);
  shader->setVec3("objectColor", color);
  shader->setFloat("transparency", transparency);
  shader->setInt("materialType", materialType); // Pass material type

  // Use texture if available
  if (texture) {
    texture->bind(0);
    shader->setBool("useTexture", true);
    shader->setInt("textureSampler", 0); // Texture is bound to slot 0
  } else {
    shader->setBool("useTexture", false);
  }

  shader->setFloat("shininess", 32.0f);
  mesh->draw();

  // Unbind texture
  if (texture) {
    texture->unbind();
  }
}

void GameObject::update(float deltaTime) {
  // Base update does nothing by default
}

void GameObject::updateBoundingBox() {
  glm::vec3 size = transform.scale;
  boundingBox = Physics::createAABBFromTransform(transform.position, size);
}

void GameObject::updateBoundingSphere(float radius) {
  boundingSphere =
      Physics::createSphereFromTransform(transform.position, radius);
}

// Pendulum Implementation
Pendulum::Pendulum(const glm::vec3 &pivot, float len)
    : GameObject(GameObjectType::PENDULUM), pivotPoint(pivot), length(len),
      swingAngle(0.0f), swingSpeed(2.0f), maxAngle(45.0f) {

  mesh.reset(Mesh::createCube(1.0f));
  color = glm::vec3(0.15f, 0.15f, 0.18f); // Dark metallic finish
  transform.scale = glm::vec3(0.3f, 2.0f, 1.5f);

  useSphereCollision = false;
}

void Pendulum::update(float deltaTime) {
  swingAngle += swingSpeed * deltaTime;

  float angle = sin(swingAngle) * glm::radians(maxAngle);

  // Calculate pendulum position
  transform.position.x = pivotPoint.x + sin(angle) * length;
  transform.position.y = pivotPoint.y - cos(angle) * length;
  transform.position.z = pivotPoint.z;

  // Rotate the blade
  transform.setRotation(glm::vec3(0.0f, 0.0f, angle));

  updateBoundingBox();
}

void Pendulum::draw(Shader *shader) { GameObject::draw(shader); }

// Crumbling Tile Implementation
CrumblingTile::CrumblingTile(const glm::vec3 &position)
    : GameObject(GameObjectType::CRUMBLING_TILE), isTriggered(false),
      shakeTimer(0.0f), fallTimer(1.0f), hasFallen(false) {

  transform.position = position;
  originalPosition = position;
  transform.scale = glm::vec3(1.0f, 0.1f, 1.0f);

  mesh.reset(Mesh::createCube(1.0f));
  color = glm::vec3(0.6f, 0.55f, 0.5f); // Distinct cracked stone color

  isTrigger = true;
  updateBoundingBox();
}

void CrumblingTile::update(float deltaTime) {
  if (!isTriggered || hasFallen)
    return;

  shakeTimer += deltaTime;

  // Shake for 2 seconds, then fall rapidly
  if (shakeTimer < 2.0f) {
    // Shake effect
    float shakeAmount = 0.05f;
    transform.position.x =
        originalPosition.x + (sin(shakeTimer * 20.0f) * shakeAmount);
    transform.position.z =
        originalPosition.z + (cos(shakeTimer * 15.0f) * shakeAmount);
  } else if (shakeTimer < 2.3f) {
    // Fall rapidly for 0.3 seconds
    transform.position.y -= 30.0f * deltaTime; // Fast fall
  } else {
    // Completely disappear - player will fall through
    hasFallen = true;
    isActive = false;
  }

  updateBoundingBox();
}

void CrumblingTile::onTrigger() {
  if (!isTriggered) {
    isTriggered = true;
    shakeTimer = 0.0f;

    // Play cracking sound
    AudioManager::getInstance().playSound(SoundEffect::TILE_CRACK, 0.7f);
  }
}

// Stalactite// Falling Stalactite
Stalactite::Stalactite(const glm::vec3 &position)
    : GameObject(GameObjectType::STALACTITE), isFalling(false),
      fallSpeed(0.0f) {

  transform.position = position;
  originalPosition = position;
  transform.scale =
      glm::vec3(1.0f, 1.0f, 1.0f); // Scale handled by cone dimensions
  transform.rotation =
      glm::vec3(glm::radians(180.0f), 0.0f, 0.0f); // Point down

  // Create cone: radius 0.6 (1.5x), height 3.75 (1.5x)
  mesh.reset(Mesh::createCone(0.6f, 3.75f, 16));
  color = glm::vec3(0.4f, 0.35f, 0.3f); // Brown rock

  // Random fall timer (between 2-8 seconds)
  fallTimer = 2.0f + (rand() % 600) / 100.0f;

  updateBoundingSphere(0.8f); // Larger hitbox
  useSphereCollision = true;
}

void Stalactite::update(float deltaTime) {
  if (!isFalling) {
    // Count down to random fall
    fallTimer -= deltaTime;
    if (fallTimer <= 0.0f) {
      isFalling = true;
    }
    return;
  }

  fallSpeed += 19.6f * deltaTime; // Gravity (2x faster)
  transform.position.y -= fallSpeed * deltaTime;

  if (transform.position.y < -10.0f) {
    isActive = false;
  }

  updateBoundingSphere(0.6f); // Larger hitbox when falling
}

void Stalactite::onTrigger() {
  if (!isFalling) {
    isFalling = true;
  }
}

// Geyser Implementation
Geyser::Geyser(const glm::vec3 &position)
    : GameObject(GameObjectType::GEYSER), eruptTimer(0.0f), eruptDuration(2.0f),
      eruptInterval(2.5f), isErupting(false) {

  transform.position = position;
  transform.scale = glm::vec3(1.2f, 0.15f, 1.2f); // Reduced by half

  mesh.reset(Mesh::createCylinder(1.5f, 0.15f,
                                  16)); // Much larger radius for bigger hitbox
  color = glm::vec3(0.25f, 0.2f, 0.2f); // Dark wet rock

  pushForce = glm::vec3(0.0f, 15.0f, 0.0f);

  isTrigger = true;
  updateBoundingBox();
}

void Geyser::update(float deltaTime) {
  eruptTimer += deltaTime;

  if (!isErupting && eruptTimer >= eruptInterval) {
    isErupting = true;
    eruptTimer = 0.0f;
  } else if (isErupting && eruptTimer >= eruptDuration) {
    isErupting = false;
    eruptTimer = 0.0f;
  }
}

void Geyser::draw(Shader *shader) {
  GameObject::draw(shader);

  // Visual indicator when erupting - make geyser glow
  if (isErupting) {
    shader->setVec3("objectColor",
                    glm::vec3(0.5f, 0.4f, 0.3f)); // Lighter when erupting
    shader->setFloat("shininess", 64.0f);         // More shiny
  }
}

// Collectible Implementation
Collectible::Collectible(const glm::vec3 &position, const glm::vec3 &color)
    : GameObject(GameObjectType::COLLECTIBLE) {
  transform.position = position;
  this->color = color;

  // Create diamond/crystal mesh - LARGER for easier collection
  mesh.reset(Mesh::createSphere(0.8f, 8, 8)); // Bigger low poly sphere
  updateBoundingSphere(1.2f); // Even larger hitbox for reliable collection

  rotationSpeed = 2.0f;
  floatOffset = (rand() % 100) / 100.0f * 3.14f;
  floatSpeed = 1.0f;
  isCollected = false;
  collectAnimation = 0.0f;
  initialScale = glm::vec3(1.0f);
  shrinkSpeed = 3.0f; // Default fast shrink
  isTrigger = true;
  useSphereCollision = true; // CRITICAL: Enable sphere collision detection!
}

void Collectible::update(float deltaTime) {
  if (isCollected) {
    collectAnimation += deltaTime * shrinkSpeed;

    // Spin and shrink
    transform.rotate(deltaTime * rotationSpeed * 5.0f,
                     glm::vec3(0.0f, 1.0f, 0.0f));
    float scale = std::max(0.0f, 1.0f - collectAnimation);
    transform.scale = initialScale * scale;

    if (collectAnimation >= 1.0f) {
      isActive = false;
    }
  } else {
    // Float and rotate
    floatOffset += floatSpeed * deltaTime;
    float yOffset = sin(floatOffset) * 0.2f;
    transform.position.y += yOffset * deltaTime;

    transform.rotate(deltaTime * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

    // Update initial scale if not collected (to capture any external scale
    // changes)
    if (collectAnimation == 0.0f) {
      // This is tricky. If we update initialScale here, we might overwrite
      // external sets. Better to assume external code sets transform.scale and
      // we capture it once or respect it. Actually, let's just use
      // transform.scale as initialScale when collect() is called.
    }
  }

  updateBoundingSphere(
      1.2f * transform.scale.x); // Much larger hitbox for reliable collection
}

void Collectible::draw(Shader *shader) {
  if (!isActive)
    return;

  glm::mat4 model = transform.getModelMatrix();
  shader->setMat4("model", model);

  // Calculate normal matrix for proper lighting
  glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
  shader->setMat3("normalMatrix", normalMatrix);

  shader->setVec3("objectColor", color);
  shader->setBool("useTexture", false);
  shader->setFloat("transparency", 1.0f);
  shader->setInt("materialType", 0);
  shader->setFloat("shininess", 256.0f); // EXTRA SHINY for glow
  mesh->draw();
}

void Collectible::collect() {
  if (!isCollected) {
    isCollected = true;
    collectAnimation = 0.0f;
    initialScale = transform.scale; // Capture current scale to shrink from

    // Play collectible pickup sound
    AudioManager::getInstance().playSound(SoundEffect::COLLECTIBLE_PICKUP,
                                          0.9f);
  }
}

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "AudioManager.h"
#include "Mesh.h"
#include "Model.h"
#include "Physics.h"
#include "Texture.h"
#include "Transform.h"
#include <memory>
#include <string>

class Shader; // Forward declaration

enum class GameObjectType {
  STATIC_WALL,
  PENDULUM,
  CRUMBLING_TILE,
  STALACTITE,
  GEYSER,
  COLLECTIBLE,
  DOOR,
  PEDESTAL,
  HEALTH_PICKUP
};

class GameObject {
public:
  Transform transform;
  std::unique_ptr<Mesh> mesh;
  std::unique_ptr<Model> model;       // Optional external model (owned)
  std::shared_ptr<Model> sharedModel; // Optional cached/shared model
  GameObjectType type;

  glm::vec3 color;
  float transparency;
  float emissive;   // 0.0 = normal shading, 1.0 = fully emissive (glowing)
  int materialType; // 0=None, 1=Brick, 2=Checkered, 3=Rock
  Texture *texture; // Optional texture for the object
  bool isActive;
  bool isTrigger;

  // Collision
  AABB boundingBox;
  Sphere boundingSphere;
  bool useSphereCollision;

  GameObject(GameObjectType type);
  virtual ~GameObject() = default;

  void loadModel(const std::string &path);
  void
  loadCachedModel(const std::string &path); // Use cached model for efficiency

  virtual void update(float deltaTime);
  virtual void draw(Shader *shader);
  virtual void onTrigger() {}

  void updateBoundingBox();
  void updateBoundingSphere(float radius);
};

// Swinging Pendulum
class Pendulum : public GameObject {
public:
  float swingAngle;
  float swingSpeed;
  float maxAngle;
  glm::vec3 pivotPoint;
  float length;

  Pendulum(const glm::vec3 &pivot, float length);
  void update(float deltaTime) override;
  void draw(Shader *shader) override;
};

// Crumbling Tile
class CrumblingTile : public GameObject {
public:
  bool isTriggered;
  float shakeTimer;
  float fallTimer;
  glm::vec3 originalPosition;
  bool hasFallen;

  CrumblingTile(const glm::vec3 &position);
  void update(float deltaTime) override;
  void onTrigger() override;
};

// Falling Stalactite
class Stalactite : public GameObject {
public:
  bool isFalling;
  float fallSpeed;
  float fallTimer; // Timer for random falling
  glm::vec3 originalPosition;

  Stalactite(const glm::vec3 &position);
  void update(float deltaTime) override;
  void onTrigger() override;
};

// Geyser Vent
class Geyser : public GameObject {
public:
  float eruptTimer;
  float eruptDuration;
  float eruptInterval;
  bool isErupting;
  glm::vec3 pushForce;

  Geyser(const glm::vec3 &position);
  void update(float deltaTime) override;
  void draw(Shader *shader) override;
};

// Collectible (Crystal/Gemstone)

// ... (existing includes)

// ... (existing classes)

// Collectible (Crystal/Gemstone)
class Collectible : public GameObject {
public:
  float rotationSpeed;
  float floatOffset;
  float floatSpeed;
  bool isCollected;
  float collectAnimation;
  glm::vec3 initialScale;
  float shrinkSpeed;
  SoundEffect soundType; // Specific sound for this collectible (COIN vs GEM)

  // std::unique_ptr<Model> model; // Optional external model - moved to
  // GameObject

  Collectible(const glm::vec3 &position, const glm::vec3 &color);
  // void loadModel(const std::string& path); // Moved to GameObject
  void update(float deltaTime) override;
  void draw(Shader *shader) override;
  void collect();
};

// Health Pickup (Heart power-up)
class HealthPickup : public GameObject {
public:
  float rotationSpeed;
  float floatOffset;
  float floatSpeed;
  bool isCollected;
  glm::vec3 basePosition;

  HealthPickup(const glm::vec3 &position);
  void update(float deltaTime) override;
  void draw(Shader *shader) override;
  void collect();
};

#endif

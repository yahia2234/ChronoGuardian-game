#ifndef LEVEL_H
#define LEVEL_H

#include "GameObject.h"
#include "ParticleSystem.h"
#include "Player.h"
#include <memory>
#include <vector>

struct Light {
  glm::vec3 position;
  glm::vec3 color;
  float intensity;

  // For flickering
  float flickerSpeed;
  float flickerAmount;
  float flickerOffset;

  Light()
      : position(0.0f), color(1.0f), intensity(1.0f), flickerSpeed(0.0f),
        flickerAmount(0.0f), flickerOffset(0.0f) {}
};

class Level {
public:
  std::vector<std::unique_ptr<GameObject>> objects;
  std::vector<std::unique_ptr<GameObject>> walls;
  std::vector<Light> lights;

  glm::vec3 ambientLight;
  glm::vec3 playerStartPosition;

  bool levelComplete;
  bool hasCollectible;
  bool shouldRestart; // Flag to signal level should restart

  Level();
  virtual ~Level() = default;

  virtual void init() = 0;
  virtual void update(float deltaTime, Player *player,
                      ParticleSystem *particles);
  virtual void draw(class Shader *shader);
  virtual void drawLights(class Shader *shader);

  void checkCollisions(Player *player, ParticleSystem *particles);
  void checkCameraCollision(glm::vec3 &cameraPos, const glm::vec3 &targetPos);
  void checkTriggers(Player *player);

protected:
  void createWall(const glm::vec3 &position, const glm::vec3 &scale,
                  const glm::vec3 &color, int materialType = 0);
  void createFloor(const glm::vec3 &position, const glm::vec3 &scale,
                   const glm::vec3 &color);
};

#endif

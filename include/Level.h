#ifndef LEVEL_H
#define LEVEL_H

#include "GameObject.h"
#include "Model.h"
#include "ParticleSystem.h"
#include "Player.h"
#include <memory>
#include <vector>

struct Light {
  glm::vec3 position;
  glm::vec3 color;
  float intensity;
  float baseIntensity; // Added for stable flickering

  // For flickering
  float flickerSpeed;
  float flickerAmount;
  float flickerOffset;

  Light()
      : position(0.0f), color(1.0f), intensity(1.0f), baseIntensity(1.0f),
        flickerSpeed(0.0f), flickerAmount(0.0f), flickerOffset(0.0f) {}
};

class Level {
public:
  std::vector<std::unique_ptr<GameObject>> objects;
  std::vector<std::unique_ptr<GameObject>> walls;
  std::vector<std::unique_ptr<GameObject>> lightFixtures;  // For fallback/extra parts
  std::vector<Light> lights;
  
  // Light fixture model (shared across all fixtures)
  std::unique_ptr<Model> lightFixtureModel;
  std::vector<glm::mat4> lightFixtureTransforms;  // Position/scale for each fixture

  glm::vec3 ambientLight;
  glm::vec3 playerStartPosition;

  bool levelComplete;
  bool hasCollectible;
  bool shouldRestart;       // Flag to signal level should restart
  bool shouldResetToLevel1; // Flag to signal reset to Level 1

  Level();
  virtual ~Level() = default;

  virtual void init() = 0;
  virtual void update(float deltaTime, Player *player,
                      ParticleSystem *particles);
  virtual void draw(class Shader *shader);
  virtual void drawLights(class Shader *shader);
  void drawLightFixtureModels(class Shader *shader);  // Draw the orb models

  void checkCollisions(Player *player, ParticleSystem *particles);
  void checkCameraCollision(glm::vec3 &cameraPos, const glm::vec3 &targetPos);
  void checkTriggers(Player *player);

protected:
  void loadLightFixtureModel();  // Load the fractured orb model
  void createWall(const glm::vec3 &position, const glm::vec3 &scale,
                  const glm::vec3 &color, int materialType = 0);
  void createFloor(const glm::vec3 &position, const glm::vec3 &scale,
                   const glm::vec3 &color);
  void createLightFixture(const glm::vec3 &position, const glm::vec3 &lightColor,
                          float scale = 1.0f);
};

#endif

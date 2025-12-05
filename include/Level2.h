#ifndef LEVEL2_H
#define LEVEL2_H

#include "Level.h"

class Level2 : public Level {
public:
  Level2();
  void init() override;
  void update(float deltaTime, Player *player,
              ParticleSystem *particles) override;

private:
  void createCavern();
  void createStalactites();
  void createGeysers();
  void createCollectible();
  void createPedestal();
  void setupLighting();

  GameObject *pedestal;
  GameObject *gemCollectible; // Reference to the gem for cutscene
  bool gemPlaced;
  int stalactiteHits; // Track number of stalactite hits

  // Ending cutscene variables
  bool endingCutscene;
  float cutsceneTimer;
  glm::vec3 gemOriginalPos;
  float gemShakeOffset;
  float pedestalRiseAmount;
};

#endif

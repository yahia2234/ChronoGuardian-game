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
  bool gemPlaced;
};

#endif

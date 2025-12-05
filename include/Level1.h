#ifndef LEVEL1_H
#define LEVEL1_H

#include "Level.h"

class Level1 : public Level {
public:
  Level1();
  void init() override;
  void update(float deltaTime, Player *player,
              ParticleSystem *particles) override;

private:
  void createChamber();
  void createMazeWalls();
  void createPendulums();
  void createCrumblingTiles();
  void createCollectible();
  void createForceFieldDoor();
  void createCheckeredFloor();

  GameObject *forceFieldDoor;
  int coinsCollected; // Track number of coins collected
};

#endif

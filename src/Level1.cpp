#include "Level1.h"
#include <iostream>

Level1::Level1()
    : forceFieldDoor(nullptr), mainGem(nullptr), shardsCollected(0),
      mainGemActive(false) {
  levelComplete = false;
  // Spawn in corner opposite to door (Door is at z=28)
  // Level is 60x60, so corners are at +/- 30
  playerStartPosition = glm::vec3(-25.0f, 2.0f, -25.0f);
}

void Level1::init() {
  createChamber();
  createMazeWalls();
  createPendulums();
  createCrumblingTiles();
  createForceFieldDoor();
  createCollectible(); // Creates 6 coins AND main gem

  // Setup lights
  lights.clear();

  // VERY BRIGHT LIGHTING - Increased intensity for wall visibility
  GLfloat ambientLight[] = {0.7f, 0.7f, 0.8f, 1.0f};   // Much brighter ambient
  GLfloat diffuseLight[] = {1.2f, 1.2f, 1.3f, 1.0f};   // Very bright diffuse
  GLfloat specularLight[] = {0.8f, 0.8f, 0.8f, 1.0f};  // Bright specular
  GLfloat lightPosition[] = {0.0f, 18.0f, 0.0f, 1.0f}; // High overhead

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);

  // Light 1 - North wall illumination
  GLfloat wallLight[] = {0.6f, 0.6f, 0.7f, 1.0f};
  GLfloat brightDiffuse[] = {1.0f, 1.0f, 1.1f, 1.0f};
  GLfloat lightPos2[] = {0.0f, 8.0f, -40.0f, 1.0f};
  glLightfv(GL_LIGHT1, GL_AMBIENT, wallLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, brightDiffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
  glEnable(GL_LIGHT1);

  // Light 2 - West wall illumination
  GLfloat lightPos3[] = {-40.0f, 8.0f, 0.0f, 1.0f};
  glLightfv(GL_LIGHT2, GL_AMBIENT, wallLight);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, brightDiffuse);
  glLightfv(GL_LIGHT2, GL_POSITION, lightPos3);
  glEnable(GL_LIGHT2);

  // Light 3 - East wall illumination
  GLfloat lightPos4[] = {40.0f, 8.0f, 0.0f, 1.0f};
  glLightfv(GL_LIGHT3, GL_AMBIENT, wallLight);
  glLightfv(GL_LIGHT3, GL_DIFFUSE, brightDiffuse);
  glLightfv(GL_LIGHT3, GL_POSITION, lightPos4);
  glEnable(GL_LIGHT3);

  // Light 4 - South wall illumination
  GLfloat lightPos5[] = {0.0f, 8.0f, 40.0f, 1.0f};
  glLightfv(GL_LIGHT4, GL_AMBIENT, wallLight);
  glLightfv(GL_LIGHT4, GL_DIFFUSE, brightDiffuse);
  glLightfv(GL_LIGHT4, GL_POSITION, lightPos5);
  glEnable(GL_LIGHT4);

  // Light 5 - Northwest corridor
  GLfloat corridorLight[] = {0.5f, 0.5f, 0.6f, 1.0f};
  GLfloat corridorDiffuse[] = {0.9f, 0.9f, 1.0f, 1.0f};
  GLfloat lightPos6[] = {-20.0f, 7.0f, -20.0f, 1.0f};
  glLightfv(GL_LIGHT5, GL_AMBIENT, corridorLight);
  glLightfv(GL_LIGHT5, GL_DIFFUSE, corridorDiffuse);
  glLightfv(GL_LIGHT5, GL_POSITION, lightPos6);
  glEnable(GL_LIGHT5);

  // Light 6 - Northeast corridor
  GLfloat lightPos7[] = {20.0f, 7.0f, -20.0f, 1.0f};
  glLightfv(GL_LIGHT6, GL_AMBIENT, corridorLight);
  glLightfv(GL_LIGHT6, GL_DIFFUSE, corridorDiffuse);
  glLightfv(GL_LIGHT6, GL_POSITION, lightPos7);
  glEnable(GL_LIGHT6);

  // Light 7 - Central gauntlet
  GLfloat lightPos8[] = {0.0f, 7.0f, 10.0f, 1.0f};
  glLightfv(GL_LIGHT7, GL_AMBIENT, corridorLight);
  glLightfv(GL_LIGHT7, GL_DIFFUSE, corridorDiffuse);
  glLightfv(GL_LIGHT7, GL_POSITION, lightPos8);
  glEnable(GL_LIGHT7);

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void Level1::createChamber() {
  // EXPANDED ROOM: 90x90 (1.5x scale from 60x60)
  float roomWidth = 90.0f;
  float roomHeight = 20.0f;
  float roomDepth = 90.0f;

  // Outer walls - High definition (Darker stone)
  glm::vec3 outerWallColor(0.4f, 0.4f, 0.45f);
  glm::vec3 borderColor(0.3f, 0.3f, 0.35f); // Darker border

  // Main Walls
  createWall(glm::vec3(0.0f, roomHeight / 2, roomDepth / 2),
             glm::vec3(roomWidth, roomHeight, 1.0f), outerWallColor); // Back
  createWall(glm::vec3(0.0f, roomHeight / 2, -roomDepth / 2),
             glm::vec3(roomWidth, roomHeight, 1.0f), outerWallColor); // Front
  createWall(glm::vec3(roomWidth / 2, roomHeight / 2, 0.0f),
             glm::vec3(1.0f, roomHeight, roomDepth), outerWallColor); // Right
  createWall(glm::vec3(-roomWidth / 2, roomHeight / 2, 0.0f),
             glm::vec3(1.0f, roomHeight, roomDepth), outerWallColor); // Left

  // Borders/Pillars for definition
  float pSize = 3.0f;
  // Corners
  createWall(
      glm::vec3(-roomWidth / 2 + 1.5f, roomHeight / 2, -roomDepth / 2 + 1.5f),
      glm::vec3(pSize, roomHeight, pSize), borderColor);
  createWall(
      glm::vec3(roomWidth / 2 - 1.5f, roomHeight / 2, -roomDepth / 2 + 1.5f),
      glm::vec3(pSize, roomHeight, pSize), borderColor);
  createWall(
      glm::vec3(-roomWidth / 2 + 1.5f, roomHeight / 2, roomDepth / 2 - 1.5f),
      glm::vec3(pSize, roomHeight, pSize), borderColor);
  createWall(
      glm::vec3(roomWidth / 2 - 1.5f, roomHeight / 2, roomDepth / 2 - 1.5f),
      glm::vec3(pSize, roomHeight, pSize), borderColor);

  // Ceiling
  createWall(glm::vec3(0.0f, roomHeight, 0.0f),
             glm::vec3(roomWidth, 1.0f, roomDepth), outerWallColor);

  // Floor - Dark Checkered
  auto floor = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
  floor->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
  floor->transform.scale = glm::vec3(roomWidth, 0.1f, roomDepth);
  floor->mesh.reset(Mesh::createCube(1.0f));
  floor->color = glm::vec3(0.2f, 0.2f, 0.25f);
  floor->materialType = 2; // Checkered
  floor->updateBoundingBox();
  walls.push_back(std::move(floor));

  // Invisible floor boundary
  createWall(glm::vec3(0.0f, -0.5f, 0.0f),
             glm::vec3(roomWidth, 0.5f, roomDepth), outerWallColor);
}

void Level1::createMazeWalls() {
  // EXPANDED MAZE - BRICK TEXTURE (Scaled for 90x90)
  glm::vec3 brickColor(0.7f, 0.3f, 0.2f); // Reddish brick
  float h = 6.0f;                         // Wall height

  // === ZONE 1: NORTHWEST (Spawn Area) - Expanded ===
  createWall(glm::vec3(-30.0f, h / 2, -30.0f), glm::vec3(1.0f, h, 30.0f),
             brickColor, 1);
  createWall(glm::vec3(-15.0f, h / 2, -37.0f), glm::vec3(30.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(-22.0f, h / 2, -22.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(-37.0f, h / 2, -15.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);

  // === ZONE 2: NORTHEAST - Expanded ===
  createWall(glm::vec3(30.0f, h / 2, -30.0f), glm::vec3(1.0f, h, 30.0f),
             brickColor, 1);
  createWall(glm::vec3(15.0f, h / 2, -37.0f), glm::vec3(30.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(22.0f, h / 2, -22.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(37.0f, h / 2, -15.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);

  // === CENTRAL GAUNTLET (Main Corridor) - Extended ===
  createWall(glm::vec3(-9.0f, h / 2, -15.0f), glm::vec3(1.0f, h, 60.0f),
             brickColor, 1);
  createWall(glm::vec3(9.0f, h / 2, -15.0f), glm::vec3(1.0f, h, 60.0f),
             brickColor, 1);

  // === ZONE 3: WEST WING - Expanded ===
  createWall(glm::vec3(-22.0f, h / 2, 0.0f), glm::vec3(27.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(-22.0f, h / 2, 15.0f), glm::vec3(1.0f, h, 30.0f),
             brickColor, 1);
  createWall(glm::vec3(-37.0f, h / 2, 22.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  createWall(glm::vec3(-30.0f, h / 2, 30.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  // Additional west maze complexity
  createWall(glm::vec3(-30.0f, h / 2, 10.0f), glm::vec3(1.0f, h, 10.0f),
             brickColor, 1);

  // === ZONE 4: EAST WING - Expanded ===
  createWall(glm::vec3(22.0f, h / 2, 0.0f), glm::vec3(27.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(22.0f, h / 2, 15.0f), glm::vec3(1.0f, h, 30.0f),
             brickColor, 1);
  createWall(glm::vec3(37.0f, h / 2, 22.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  createWall(glm::vec3(30.0f, h / 2, 30.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  // Additional east maze complexity
  createWall(glm::vec3(30.0f, h / 2, 10.0f), glm::vec3(1.0f, h, 10.0f),
             brickColor, 1);

  // === ZONE 5: SOUTH (Exit Area) - Expanded ===
  createWall(glm::vec3(-15.0f, h / 2, 33.0f), glm::vec3(12.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(15.0f, h / 2, 33.0f), glm::vec3(12.0f, h, 1.0f),
             brickColor, 1);

  // Additional maze complexity - more walls for challenge
  createWall(glm::vec3(0.0f, h / 2, -7.0f), glm::vec3(12.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(-15.0f, h / 2, 7.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  createWall(glm::vec3(15.0f, h / 2, 7.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  // Cross passages
  createWall(glm::vec3(-30.0f, h / 2, -7.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  createWall(glm::vec3(30.0f, h / 2, -7.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
}

void Level1::createPendulums() {
  // SWINGING BLOCK PENDULUMS - More pendulums, faster speeds

  // 1. The Gauntlet (Central Corridor) - 8 pendulums (increased from 6)
  for (int i = 0; i < 8; i++) {
    float zPos = -18.0f + (i * 7.0f);
    auto p = std::make_unique<Pendulum>(glm::vec3(0.0f, 8.0f, zPos), 6.0f);
    p->swingSpeed = 2.0f + (i % 3) * 0.5f; // Increased speed
    p->swingAngle = i * 1.0f;
    p->transform.scale = glm::vec3(6.0f, 4.0f, 0.5f);
    p->updateBoundingBox();
    objects.push_back(std::move(p));
  }

  // 2. West Wing Guards - 4 pendulums (increased from 2)
  auto p1 = std::make_unique<Pendulum>(glm::vec3(-22.0f, 8.0f, -15.0f), 5.0f);
  p1->swingSpeed = 2.5f; // Faster
  p1->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p1->updateBoundingBox();
  objects.push_back(std::move(p1));

  auto p2 = std::make_unique<Pendulum>(glm::vec3(-30.0f, 8.0f, 5.0f), 5.0f);
  p2->swingSpeed = 2.5f;
  p2->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p2->updateBoundingBox();
  objects.push_back(std::move(p2));

  auto p3 = std::make_unique<Pendulum>(glm::vec3(-22.0f, 8.0f, 15.0f), 5.0f);
  p3->swingSpeed = 2.0f;
  p3->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p3->updateBoundingBox();
  objects.push_back(std::move(p3));

  auto p4 = std::make_unique<Pendulum>(glm::vec3(-30.0f, 8.0f, 25.0f), 5.0f);
  p4->swingSpeed = 2.5f;
  p4->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p4->updateBoundingBox();
  objects.push_back(std::move(p4));

  // 3. East Wing Guards - 4 pendulums (increased from 2)
  auto p5 = std::make_unique<Pendulum>(glm::vec3(22.0f, 8.0f, -15.0f), 5.0f);
  p5->swingSpeed = 2.5f;
  p5->swingAngle = 3.14f;
  p5->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p5->updateBoundingBox();
  objects.push_back(std::move(p5));

  auto p6 = std::make_unique<Pendulum>(glm::vec3(30.0f, 8.0f, 5.0f), 5.0f);
  p6->swingSpeed = 2.5f;
  p6->swingAngle = 3.14f;
  p6->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p6->updateBoundingBox();
  objects.push_back(std::move(p6));

  auto p7 = std::make_unique<Pendulum>(glm::vec3(22.0f, 8.0f, 15.0f), 5.0f);
  p7->swingSpeed = 2.0f;
  p7->swingAngle = 3.14f;
  p7->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p7->updateBoundingBox();
  objects.push_back(std::move(p7));

  auto p8 = std::make_unique<Pendulum>(glm::vec3(30.0f, 8.0f, 25.0f), 5.0f);
  p8->swingSpeed = 2.5f;
  p8->swingAngle = 3.14f;
  p8->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p8->updateBoundingBox();
  objects.push_back(std::move(p8));
}

void Level1::createCrumblingTiles() {
  // Strategic trap tiles at key intersections

  // Northwest zone entrance
  objects.push_back(
      std::make_unique<CrumblingTile>(glm::vec3(-12.0f, 0.1f, -18.0f)));

  // Northeast zone entrance
  objects.push_back(
      std::make_unique<CrumblingTile>(glm::vec3(12.0f, 0.1f, -18.0f)));

  // West wing corridor
  objects.push_back(
      std::make_unique<CrumblingTile>(glm::vec3(-18.0f, 0.1f, 5.0f)));

  // East wing corridor
  objects.push_back(
      std::make_unique<CrumblingTile>(glm::vec3(18.0f, 0.1f, 5.0f)));

  // Near exit
  objects.push_back(
      std::make_unique<CrumblingTile>(glm::vec3(0.0f, 0.1f, 20.0f)));
}

void Level1::createCollectible() {
  // 6 Coins strategically placed throughout the expanded maze (Ground level
  // y=1.0)

  // 1. Northwest zone - Deep corner (requires exploring spawn area)
  auto c1 = std::make_unique<Collectible>(glm::vec3(-23.0f, 1.0f, -23.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c1->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c1->rotationSpeed = 6.0f;
  objects.push_back(std::move(c1));

  // 2. Northeast zone - Behind pendulum (challenging)
  auto c2 = std::make_unique<Collectible>(glm::vec3(23.0f, 1.0f, -18.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c2->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c2->rotationSpeed = 6.0f;
  objects.push_back(std::move(c2));

  // 3. West wing - Dead end (rewards exploration)
  auto c3 = std::make_unique<Collectible>(glm::vec3(-23.0f, 1.0f, 18.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c3->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c3->rotationSpeed = 6.0f;
  objects.push_back(std::move(c3));

  // 4. East wing - Dead end (symmetric to west)
  auto c4 = std::make_unique<Collectible>(glm::vec3(23.0f, 1.0f, 18.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c4->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c4->rotationSpeed = 6.0f;
  objects.push_back(std::move(c4));

  // 5. Central area - Mid gauntlet (requires timing through pendulums)
  auto c5 = std::make_unique<Collectible>(glm::vec3(0.0f, 1.0f, 0.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c5->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c5->rotationSpeed = 6.0f;
  objects.push_back(std::move(c5));

  // 6. South zone - Near exit (final coin before gem)
  auto c6 = std::make_unique<Collectible>(glm::vec3(0.0f, 1.0f, 18.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c6->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c6->rotationSpeed = 6.0f;
  objects.push_back(std::move(c6));

  // MAIN GEM (Energy Crystal) - At the very end
  auto gem = std::make_unique<Collectible>(glm::vec3(0.0f, 1.0f, 25.0f),
                                           glm::vec3(0.0f, 1.0f, 1.0f));
  gem->isActive = false; // Hidden until coins collected
  mainGem = gem.get();   // Store raw pointer
  objects.push_back(std::move(gem));
}

void Level1::createForceFieldDoor() {
  // Create stone archway frame
  glm::vec3 frameColor(0.5f, 0.5f, 0.55f); // Stone color

  // Door moved to z=28.0 (Room depth is 60, so wall is at 30. 28 gives space)
  float doorZ = 28.0f;

  // Left pillar
  createWall(glm::vec3(-1.5f, 2.5f, doorZ), glm::vec3(0.5f, 5.0f, 0.5f),
             frameColor);
  // Right pillar
  createWall(glm::vec3(1.5f, 2.5f, doorZ), glm::vec3(0.5f, 5.0f, 0.5f),
             frameColor);
  // Top arch
  createWall(glm::vec3(0.0f, 5.25f, doorZ), glm::vec3(3.5f, 0.5f, 0.5f),
             frameColor);

  // The force field itself
  auto door = std::make_unique<GameObject>(GameObjectType::DOOR);
  door->transform.position = glm::vec3(0.0f, 2.5f, doorZ);
  door->transform.scale =
      glm::vec3(2.5f, 5.0f, 0.1f); // Slightly thinner than frame
  door->mesh.reset(Mesh::createCube(1.0f));
  door->color = glm::vec3(0.2f, 0.6f, 1.0f); // Glowing blue force field
  door->transparency = 0.2f; // More transparent to show it's open
  door->isActive = false;    // ALWAYS OPEN
  door->updateBoundingBox();

  forceFieldDoor = door.get();
  walls.push_back(std::move(door));
}

void Level1::update(float deltaTime, Player *player,
                    ParticleSystem *particles) {
  // Custom update logic to track shards
  for (auto &obj : objects) {
    if (!obj->isActive || !obj->isTrigger)
      continue;

    if (obj->type == GameObjectType::COLLECTIBLE) {
      if (Physics::checkSphereCollision(player->collisionSphere,
                                        obj->boundingSphere)) {
        auto collectible = static_cast<Collectible *>(obj.get());
        if (!collectible->isCollected) {
          collectible->collect();

          if (collectible == mainGem) {
            hasCollectible = true;
          } else {
            shardsCollected++;

            // Emit Yellow Flare
            particles->emitExplosion(collectible->transform.position,
                                     glm::vec3(1.0f, 0.8f, 0.2f), 20);

            // Check if all shards collected
            if (shardsCollected >= totalShards && mainGem) {
              mainGem->isActive = true;
            }
          }
        }
      }
    }
  }

  Level::update(deltaTime, player, particles);

  // Check if player reached the exit (Door is always open now)
  // Trigger just past the door (door is at 28.0 now)
  if (player->getPosition().z > 28.5f) {
    levelComplete = true;
  }
}

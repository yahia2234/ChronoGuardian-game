#include "Level1.h"

Level1::Level1() : forceFieldDoor(nullptr) {
  levelComplete = false;
  // Spawn in corner opposite to door (Door is at z=28)
  // Level is 90x90, so corners are at +/- 30
  playerStartPosition = glm::vec3(-25.0f, 2.0f, -25.0f);
}

void Level1::init() {
  createChamber();
  createMazeWalls();
  createPendulums();
  createCrumblingTiles();
  createForceFieldDoor();
  createCheckeredFloor(); // New checkered floor
  createCollectible();    // Creates 6 coins AND main gem

  // Setup lights - 8 evenly distributed static lights
  lights.clear();

  // Create 8 lights in a grid pattern (2x4) across the 90x90 room
  // Room spans from -45 to +45 in both x and z
  float lightHeight = 15.0f;              // High up for good coverage
  float lightIntensity = 3.0f;            // Bright intensity for well-lit scene
  glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // White light

  // Grid positions: 2 rows (x), 4 columns (z)
  float xPositions[] = {-22.5f, 22.5f}; // 2 positions along x-axis
  float zPositions[] = {-33.75f, -11.25f, 11.25f,
                        33.75f}; // 4 positions along z-axis

  for (float xPos : xPositions) {
    for (float zPos : zPositions) {
      Light light;
      light.position = glm::vec3(xPos, lightHeight, zPos);
      light.color = lightColor;
      light.intensity = lightIntensity;
      lights.push_back(light);
    }
  }

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

  // Floor is now created in createCheckeredFloor()

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
  // === CENTRAL GAUNTLET (Main Corridor) - Modified for Access ===
  // Split walls to create entry gaps near spawn (z = -25 to -15)

  // Left side (West)
  // Segment 1: Back
  createWall(glm::vec3(-9.0f, h / 2, -35.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);
  // Segment 2: Front (Gap from -25 to -15)
  createWall(glm::vec3(-9.0f, h / 2, 0.0f), glm::vec3(1.0f, h, 30.0f),
             brickColor, 1);

  // Right side (East)
  // Segment 1: Back
  createWall(glm::vec3(9.0f, h / 2, -35.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);
  // Segment 2: Front (Gap from -25 to -15)
  createWall(glm::vec3(9.0f, h / 2, 0.0f), glm::vec3(1.0f, h, 30.0f),
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
  // REMOVED BLOCKING WALL at (0, -7) to allow path
  // createWall(glm::vec3(0.0f, h / 2, -7.0f), glm::vec3(12.0f, h, 1.0f),
  //            brickColor, 1);
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
  // SWINGING BLOCK PENDULUMS - Significantly increased count

  // 1. The Gauntlet (Central Corridor) - 12 pendulums (increased from 8)
  for (int i = 0; i < 12; i++) {
    float zPos = -18.0f + (i * 4.5f); // Tighter spacing
    auto p = std::make_unique<Pendulum>(glm::vec3(0.0f, 8.0f, zPos), 6.0f);
    p->swingSpeed = 2.0f + (i % 4) * 0.5f;
    p->swingAngle = i * 0.8f;
    p->transform.scale = glm::vec3(6.0f, 4.0f, 0.5f);
    p->updateBoundingBox();
    objects.push_back(std::move(p));
  }

  // 2. West Wing Guards - 6 pendulums
  for (int i = 0; i < 6; i++) {
    float zPos = -15.0f + (i * 8.0f);
    auto p = std::make_unique<Pendulum>(glm::vec3(-26.0f, 8.0f, zPos), 5.0f);
    p->swingSpeed = 2.5f + (i % 2) * 0.5f;
    p->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
    p->updateBoundingBox();
    objects.push_back(std::move(p));
  }

  // 3. East Wing Guards - 6 pendulums
  for (int i = 0; i < 6; i++) {
    float zPos = -15.0f + (i * 8.0f);
    auto p = std::make_unique<Pendulum>(glm::vec3(26.0f, 8.0f, zPos), 5.0f);
    p->swingSpeed = 2.5f + (i % 2) * 0.5f;
    p->swingAngle = 3.14f;
    p->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
    p->updateBoundingBox();
    objects.push_back(std::move(p));
  }
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
  // 6 Coins placed throughout the level

  // 1. Hidden behind Northwest corner wall
  auto c1 = std::make_unique<Collectible>(glm::vec3(-40.0f, 1.0f, -40.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c1->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c1->rotationSpeed = 6.0f;
  objects.push_back(std::move(c1));

  // 2. Hidden in Northeast alcove
  auto c2 = std::make_unique<Collectible>(glm::vec3(40.0f, 1.0f, -40.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c2->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c2->rotationSpeed = 6.0f;
  objects.push_back(std::move(c2));

  // 3. Hidden in West Wing dead end
  auto c3 = std::make_unique<Collectible>(glm::vec3(-40.0f, 1.0f, 10.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c3->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c3->rotationSpeed = 6.0f;
  objects.push_back(std::move(c3));

  // 4. Hidden in East Wing dead end
  auto c4 = std::make_unique<Collectible>(glm::vec3(40.0f, 1.0f, 10.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c4->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c4->rotationSpeed = 6.0f;
  objects.push_back(std::move(c4));

  // 5. Hidden behind a pillar in the South
  auto c5 = std::make_unique<Collectible>(glm::vec3(-20.0f, 1.0f, 35.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c5->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c5->rotationSpeed = 6.0f;
  objects.push_back(std::move(c5));

  // 6. Hidden in the opposite South corner
  auto c6 = std::make_unique<Collectible>(glm::vec3(20.0f, 1.0f, 35.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c6->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c6->rotationSpeed = 6.0f;
  objects.push_back(std::move(c6));
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
  // Custom update logic to track coins
  for (auto &obj : objects) {
    if (!obj->isActive || !obj->isTrigger)
      continue;

    if (obj->type == GameObjectType::COLLECTIBLE) {
      if (Physics::checkSphereCollision(player->collisionSphere,
                                        obj->boundingSphere)) {
        auto collectible = static_cast<Collectible *>(obj.get());
        if (!collectible->isCollected) {
          collectible->collect();
          hasCollectible = true;

          // Emit particle effect
          particles->emitExplosion(collectible->transform.position,
                                   glm::vec3(1.0f, 0.8f, 0.2f), 20);
        }
      }
    }
  }

  Level::update(deltaTime, player, particles);

  // Check if player reached the exit (Door is always open)
  if (player->getPosition().z > 28.5f) {
    levelComplete = true;
  }
}

void Level1::createCheckeredFloor() {
  float roomSize = 90.0f;
  float tileSize = 5.0f;
  int tilesPerSide = static_cast<int>(roomSize / tileSize);
  float startOffset = -roomSize / 2.0f + tileSize / 2.0f;

  for (int i = 0; i < tilesPerSide; ++i) {
    for (int j = 0; j < tilesPerSide; ++j) {
      float x = startOffset + i * tileSize;
      float z = startOffset + j * tileSize;

      // Determine if this should be a crumbling tile
      // Make roughly 20% of tiles crumbling, distributed in a pattern
      bool isCrumbling = ((i + j) % 5 == 2) || ((i * 3 + j * 2) % 7 == 3);

      if (isCrumbling) {
        // Create crumbling tile with orange color at ground level
        auto crumblingTile =
            std::make_unique<CrumblingTile>(glm::vec3(x, -0.5f, z));
        crumblingTile->transform.scale = glm::vec3(tileSize, 1.0f, tileSize);
        crumblingTile->color =
            glm::vec3(0.9f, 0.5f, 0.2f); // Bright orange for crumbling tiles
        crumblingTile->originalPosition = glm::vec3(x, -0.5f, z);
        crumblingTile->fallTimer = 2.0f; // 2 seconds before falling
        crumblingTile->updateBoundingBox();
        objects.push_back(std::move(crumblingTile));
      } else {
        // Create regular static tile
        auto tile = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
        tile->transform.position = glm::vec3(x, -0.5f, z);
        tile->transform.scale = glm::vec3(tileSize, 1.0f, tileSize);
        tile->mesh.reset(Mesh::createCube(1.0f));

        // Checkered pattern with orange and grey
        if ((i + j) % 2 == 0) {
          tile->color = glm::vec3(0.5f, 0.5f, 0.5f); // Grey
        } else {
          tile->color = glm::vec3(0.7f, 0.35f, 0.15f); // Darker orange
        }

        tile->updateBoundingBox();
        walls.push_back(std::move(tile));
      }
    }
  }
}

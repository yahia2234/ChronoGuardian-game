#include "Level1.h"
#include <iostream>

Level1::Level1()
    : forceFieldDoor(nullptr), energyCrystal(nullptr), coinsCollected(0),
      crystalCollected(false), crumblingTileTexture(nullptr) {
  levelComplete = false;
  // Spawn in corner opposite to door (Door is at z=28)
  // Level is 90x90, so corners are at +/- 30
  playerStartPosition = glm::vec3(-25.0f, 2.0f, -25.0f);
}

void Level1::init() {
  // Create cracked tile texture for crumbling tiles
  crumblingTileTexture = Texture::createCrackedTile(256);
  std::cout << "Created cracked tile texture with ID: "
            << crumblingTileTexture->ID << std::endl;

  createChamber();
  createMazeWalls();
  createPendulums();
  createCrumblingTiles();
  createForceFieldDoor();
  createCheckeredFloor(); // New checkered floor
  createCollectible();    // Creates 10 coins

  // Create energy crystal (initially hidden, appears after 6 coins)
  auto crystal = std::make_unique<Collectible>(
      glm::vec3(0.0f, 2.0f, 15.0f), // Central location near exit
      glm::vec3(0.2f, 0.6f, 1.0f)   // Blue color
  );
  crystal->transform.scale = glm::vec3(1.2f, 1.2f, 0.15f); // Larger than coins
  crystal->rotationSpeed = 4.0f;                           // Slower rotation
  crystal->isActive = false;                               // Hidden initially
  crystal->isTrigger = true;
  energyCrystal = crystal.get();
  objects.push_back(std::move(crystal));

  // Setup lights - 8 evenly distributed static lights
  lights.clear();

  // Create 8 lights in a grid pattern (2x4) across the 90x90 room
  // Room spans from -45 to +45 in both x and z
  float lightHeight = 12.0f;              // High up for good coverage
  float lightIntensity = 4.0f;            // Bright intensity for well-lit scene
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
  // MAZE LAYOUT - Structured pathways with walls
  glm::vec3 brickColor(0.7f, 0.3f, 0.2f); // Reddish brick
  float h = 6.0f;                         // Wall height

  // Outer perimeter decorative walls (corners)
  // Northwest corner
  createWall(glm::vec3(-40.0f, h / 2, -35.0f), glm::vec3(1.0f, h, 10.0f),
             brickColor, 1);
  createWall(glm::vec3(-35.0f, h / 2, -40.0f), glm::vec3(10.0f, h, 1.0f),
             brickColor, 1);

  // Northeast corner
  createWall(glm::vec3(40.0f, h / 2, -35.0f), glm::vec3(1.0f, h, 10.0f),
             brickColor, 1);
  createWall(glm::vec3(35.0f, h / 2, -40.0f), glm::vec3(10.0f, h, 1.0f),
             brickColor, 1);

  // Southwest corner
  createWall(glm::vec3(-40.0f, h / 2, 35.0f), glm::vec3(1.0f, h, 10.0f),
             brickColor, 1);
  createWall(glm::vec3(-35.0f, h / 2, 40.0f), glm::vec3(10.0f, h, 1.0f),
             brickColor, 1);

  // Southeast corner
  createWall(glm::vec3(40.0f, h / 2, 35.0f), glm::vec3(1.0f, h, 10.0f),
             brickColor, 1);
  createWall(glm::vec3(35.0f, h / 2, 40.0f), glm::vec3(10.0f, h, 1.0f),
             brickColor, 1);

  // Create maze pathways - vertical walls
  createWall(glm::vec3(-25.0f, h / 2, -20.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);
  createWall(glm::vec3(-25.0f, h / 2, 10.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);

  createWall(glm::vec3(25.0f, h / 2, -20.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);
  createWall(glm::vec3(25.0f, h / 2, 10.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);

  createWall(glm::vec3(-10.0f, h / 2, -30.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  createWall(glm::vec3(-10.0f, h / 2, 5.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);

  createWall(glm::vec3(10.0f, h / 2, -30.0f), glm::vec3(1.0f, h, 15.0f),
             brickColor, 1);
  createWall(glm::vec3(10.0f, h / 2, 5.0f), glm::vec3(1.0f, h, 20.0f),
             brickColor, 1);

  // Horizontal walls to create maze paths
  createWall(glm::vec3(-35.0f, h / 2, -15.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(-35.0f, h / 2, 15.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);

  createWall(glm::vec3(35.0f, h / 2, -15.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(35.0f, h / 2, 15.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);

  createWall(glm::vec3(-17.5f, h / 2, 0.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(17.5f, h / 2, 0.0f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);

  createWall(glm::vec3(0.0f, h / 2, -22.5f), glm::vec3(15.0f, h, 1.0f),
             brickColor, 1);

  // Exit area walls (flanking the door)
  createWall(glm::vec3(-10.0f, h / 2, 35.0f), glm::vec3(8.0f, h, 1.0f),
             brickColor, 1);
  createWall(glm::vec3(10.0f, h / 2, 35.0f), glm::vec3(8.0f, h, 1.0f),
             brickColor, 1);
}

void Level1::createPendulums() {
  // SPREAD OUT PENDULUMS - Distributed across different areas

  // Northwest area - 3 pendulums
  auto p1 = std::make_unique<Pendulum>(glm::vec3(-32.0f, 8.0f, -25.0f), 6.0f);
  p1->swingSpeed = 2.0f;
  p1->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p1->updateBoundingBox();
  objects.push_back(std::move(p1));

  auto p2 = std::make_unique<Pendulum>(glm::vec3(-32.0f, 8.0f, -10.0f), 6.0f);
  p2->swingSpeed = 2.3f;
  p2->swingAngle = 1.5f;
  p2->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p2->updateBoundingBox();
  objects.push_back(std::move(p2));

  auto p3 = std::make_unique<Pendulum>(glm::vec3(-32.0f, 8.0f, 5.0f), 6.0f);
  p3->swingSpeed = 2.5f;
  p3->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p3->updateBoundingBox();
  objects.push_back(std::move(p3));

  // Northeast area - 3 pendulums
  auto p4 = std::make_unique<Pendulum>(glm::vec3(32.0f, 8.0f, -25.0f), 6.0f);
  p4->swingSpeed = 2.2f;
  p4->swingAngle = 3.0f;
  p4->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p4->updateBoundingBox();
  objects.push_back(std::move(p4));

  auto p5 = std::make_unique<Pendulum>(glm::vec3(32.0f, 8.0f, -10.0f), 6.0f);
  p5->swingSpeed = 2.4f;
  p5->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p5->updateBoundingBox();
  objects.push_back(std::move(p5));

  auto p6 = std::make_unique<Pendulum>(glm::vec3(32.0f, 8.0f, 5.0f), 6.0f);
  p6->swingSpeed = 2.1f;
  p6->swingAngle = 2.0f;
  p6->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p6->updateBoundingBox();
  objects.push_back(std::move(p6));

  // Central corridor - 4 pendulums
  auto p7 = std::make_unique<Pendulum>(glm::vec3(-17.0f, 8.0f, -7.0f), 6.0f);
  p7->swingSpeed = 2.3f;
  p7->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p7->updateBoundingBox();
  objects.push_back(std::move(p7));

  auto p8 = std::make_unique<Pendulum>(glm::vec3(17.0f, 8.0f, -7.0f), 6.0f);
  p8->swingSpeed = 2.2f;
  p8->swingAngle = 1.0f;
  p8->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p8->updateBoundingBox();
  objects.push_back(std::move(p8));

  auto p9 = std::make_unique<Pendulum>(glm::vec3(0.0f, 8.0f, 10.0f), 6.0f);
  p9->swingSpeed = 2.5f;
  p9->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p9->updateBoundingBox();
  objects.push_back(std::move(p9));

  auto p10 = std::make_unique<Pendulum>(glm::vec3(0.0f, 8.0f, 25.0f), 6.0f);
  p10->swingSpeed = 2.0f;
  p10->swingAngle = 2.5f;
  p10->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p10->updateBoundingBox();
  objects.push_back(std::move(p10));

  // Southwest area - 2 pendulums
  auto p11 = std::make_unique<Pendulum>(glm::vec3(-32.0f, 8.0f, 22.0f), 6.0f);
  p11->swingSpeed = 2.4f;
  p11->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p11->updateBoundingBox();
  objects.push_back(std::move(p11));

  // Southeast area - 2 pendulums
  auto p12 = std::make_unique<Pendulum>(glm::vec3(32.0f, 8.0f, 22.0f), 6.0f);
  p12->swingSpeed = 2.3f;
  p12->swingAngle = 1.8f;
  p12->transform.scale = glm::vec3(5.0f, 3.0f, 0.5f);
  p12->updateBoundingBox();
  objects.push_back(std::move(p12));
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
  // 10 Coins placed in open areas throughout the level (not inside walls)

  // 1. Northwest area (open space)
  auto c1 = std::make_unique<Collectible>(glm::vec3(-32.0f, 1.5f, -32.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c1->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c1->rotationSpeed = 6.0f;
  objects.push_back(std::move(c1));

  // 2. Northeast area (open space)
  auto c2 = std::make_unique<Collectible>(glm::vec3(32.0f, 1.5f, -32.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c2->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c2->rotationSpeed = 6.0f;
  objects.push_back(std::move(c2));

  // 3. Southwest area (open space)
  auto c3 = std::make_unique<Collectible>(glm::vec3(-32.0f, 1.5f, 32.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c3->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c3->rotationSpeed = 6.0f;
  objects.push_back(std::move(c3));

  // 4. Southeast area (open space)
  auto c4 = std::make_unique<Collectible>(glm::vec3(32.0f, 1.5f, 32.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c4->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c4->rotationSpeed = 6.0f;
  objects.push_back(std::move(c4));

  // 5. West corridor
  auto c5 = std::make_unique<Collectible>(glm::vec3(-17.0f, 1.5f, -25.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c5->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c5->rotationSpeed = 6.0f;
  objects.push_back(std::move(c5));

  // 6. East corridor
  auto c6 = std::make_unique<Collectible>(glm::vec3(17.0f, 1.5f, -25.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c6->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c6->rotationSpeed = 6.0f;
  objects.push_back(std::move(c6));

  // 7. Central north area
  auto c7 = std::make_unique<Collectible>(glm::vec3(0.0f, 1.5f, -32.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c7->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c7->rotationSpeed = 6.0f;
  objects.push_back(std::move(c7));

  // 8. Central area
  auto c8 = std::make_unique<Collectible>(glm::vec3(0.0f, 1.5f, -7.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c8->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c8->rotationSpeed = 6.0f;
  objects.push_back(std::move(c8));

  // 9. South central area
  auto c9 = std::make_unique<Collectible>(glm::vec3(-17.0f, 1.5f, 17.0f),
                                          glm::vec3(1.0f, 0.84f, 0.0f));
  c9->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c9->rotationSpeed = 6.0f;
  objects.push_back(std::move(c9));

  // 10. Near exit (but not too close)
  auto c10 = std::make_unique<Collectible>(glm::vec3(17.0f, 1.5f, 17.0f),
                                           glm::vec3(1.0f, 0.84f, 0.0f));
  c10->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f);
  c10->rotationSpeed = 6.0f;
  objects.push_back(std::move(c10));
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
  door->transparency = 0.7f;                 // Semi-transparent force field
  door->isActive = true; // ENABLED - blocks until 6 coins collected
  door->updateBoundingBox();

  forceFieldDoor = door.get();
  walls.push_back(std::move(door));
}

void Level1::update(float deltaTime, Player *player,
                    ParticleSystem *particles) {
  // Custom update logic to track coins and crystal
  for (auto &obj : objects) {
    if (!obj->isActive || !obj->isTrigger)
      continue;

    if (obj->type == GameObjectType::COLLECTIBLE) {
      if (Physics::checkSphereCollision(player->collisionSphere,
                                        obj->boundingSphere)) {
        auto collectible = static_cast<Collectible *>(obj.get());
        if (!collectible->isCollected) {
          collectible->collect();

          // Check if this is the energy crystal
          if (obj.get() == energyCrystal) {
            // Collected the energy crystal!
            crystalCollected = true;

            // Unlock the door
            if (forceFieldDoor) {
              forceFieldDoor->isActive = false;
              forceFieldDoor->transparency = 0.2f;
            }

            // Blue particle explosion
            particles->emitExplosion(collectible->transform.position,
                                     glm::vec3(0.2f, 0.6f, 1.0f), 30);
          } else {
            // Regular coin collected
            coinsCollected++;

            // Emit particle effect
            particles->emitExplosion(collectible->transform.position,
                                     glm::vec3(1.0f, 0.8f, 0.2f), 20);

            // Spawn energy crystal after 6 coins
            if (coinsCollected >= 6 && energyCrystal &&
                !energyCrystal->isActive) {
              energyCrystal->isActive = true;

              // Spawn effect for crystal appearing
              particles->emitExplosion(energyCrystal->transform.position,
                                       glm::vec3(0.2f, 0.6f, 1.0f), 40);
            }
          }
        }
      }
    }
  }

  Level::update(deltaTime, player, particles);

  // Check if player is standing on a disappeared crumbling tile
  glm::vec3 playerPos = player->getPosition();
  for (auto &obj : objects) {
    if (obj->type == GameObjectType::CRUMBLING_TILE) {
      auto tile = static_cast<CrumblingTile *>(obj.get());
      // If tile has fallen and player is still above its original position
      if (tile->hasFallen && !obj->isActive) {
        glm::vec3 tilePos = tile->originalPosition;
        float tileSize = 5.0f; // Match the floor tile size

        // Check if player is above the disappeared tile's original position
        float heightDiff = playerPos.y - tilePos.y;
        bool isAboveTile = (heightDiff > 0.0f && heightDiff < 3.0f);

        // Check horizontal overlap
        bool xOverlap = std::abs(playerPos.x - tilePos.x) < (tileSize / 2.0f);
        bool zOverlap = std::abs(playerPos.z - tilePos.z) < (tileSize / 2.0f);

        // Only restart if player is grounded (not jumping)
        // Check if vertical velocity is near zero (player is standing, not
        // jumping)
        bool isGrounded = std::abs(player->velocity.y) < 0.5f;

        if (isAboveTile && xOverlap && zOverlap && isGrounded) {
          // Player is standing where a tile disappeared - restart level
          shouldRestart = true;
          return;
        }
      }
    }
  }

  // Check if player passed through the door (precise collision with door
  // dimensions)
  if (forceFieldDoor && !forceFieldDoor->isActive) {
    // Door is at z=28, dimensions are 2.5 wide x 5.0 tall
    // Check if player is within door bounds and has passed through
    glm::vec3 doorPos = forceFieldDoor->transform.position;
    glm::vec3 doorScale = forceFieldDoor->transform.scale;

    float doorLeft = doorPos.x - (doorScale.x / 2.0f);
    float doorRight = doorPos.x + (doorScale.x / 2.0f);
    float doorBottom = doorPos.y - (doorScale.y / 2.0f);
    float doorTop = doorPos.y + (doorScale.y / 2.0f);

    // Check if player is within door's X and Y bounds and has passed the Z
    // threshold
    bool withinDoorX = (playerPos.x >= doorLeft && playerPos.x <= doorRight);
    bool withinDoorY = (playerPos.y >= doorBottom && playerPos.y <= doorTop);
    bool passedDoor =
        (playerPos.z > doorPos.z + 0.5f); // Passed through the door

    if (withinDoorX && withinDoorY && passedDoor) {
      levelComplete = true;
    }
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
        crumblingTile->texture = crumblingTileTexture; // Assign cracked texture

        // Debug: Print first tile assignment
        static bool printed = false;
        if (!printed && crumblingTileTexture) {
          std::cout << "Assigned texture ID " << crumblingTileTexture->ID
                    << " to crumbling tile at (" << x << ", " << z << ")"
                    << std::endl;
          printed = true;
        }

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

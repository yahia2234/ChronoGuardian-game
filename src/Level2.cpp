#include "Level2.h"

Level2::Level2() : pedestal(nullptr), gemPlaced(false) {
  playerStartPosition = glm::vec3(0.0f, 1.0f, -8.0f);
}

void Level2::init() {
  createCavern();
  createStalactites();
  createGeysers();
  createCollectible();
  createPedestal();
  setupLighting();
}

void Level2::createCavern() {
  // Enhanced earthy colors for distinct cave appearance
  glm::vec3 rockColor(0.42f, 0.32f, 0.25f); // Warmer brown cave rock

  // Expanded square cavern (60x60)
  float roomWidth = 60.0f;
  float roomHeight = 15.0f;
  float roomDepth = 60.0f; // Square layout

  // North wall (irregular rough rock)
  createWall(glm::vec3(0.0f, roomHeight / 2, roomDepth / 2),
             glm::vec3(roomWidth, roomHeight, 1.0f), rockColor);
  walls.back()->materialType = 3; // Rock texture

  // South wall
  createWall(glm::vec3(0.0f, roomHeight / 2, -roomDepth / 2),
             glm::vec3(roomWidth, roomHeight, 1.0f), rockColor);
  walls.back()->materialType = 3; // Rock texture

  // East wall (irregular rough rock)
  createWall(glm::vec3(roomWidth / 2, roomHeight / 2, 0.0f),
             glm::vec3(1.0f, roomHeight, roomDepth), rockColor);
  walls.back()->materialType = 3; // Rock texture

  // West wall
  createWall(glm::vec3(-roomWidth / 2, roomHeight / 2, 0.0f),
             glm::vec3(1.0f, roomHeight, roomDepth), rockColor);
  walls.back()->materialType = 3; // Rock texture

  // Ceiling (rough cave rock)
  createWall(glm::vec3(0.0f, roomHeight, 0.0f),
             glm::vec3(roomWidth, 1.0f, roomDepth), rockColor);
  walls.back()->materialType = 3; // Rock texture

  // Uneven dirt floor with distinct earthy color
  glm::vec3 dirtColor(0.35f, 0.28f, 0.18f); // Rich dirt brown
  createFloor(glm::vec3(0.0f, 0.0f, 0.0f),
              glm::vec3(roomWidth, 0.1f, roomDepth), dirtColor);
  walls.back()->materialType = 3; // Rock texture for ground
}

void Level2::createStalactites() {
  // Create stalactites hanging from ceiling - Many more for 60x60 space
  std::vector<glm::vec3> stalactitePositions = {
      // Northwest quadrant
      glm::vec3(-20.0f, 13.0f, -20.0f), glm::vec3(-15.0f, 13.0f, -25.0f),
      glm::vec3(-25.0f, 13.0f, -15.0f), glm::vec3(-18.0f, 13.0f, -18.0f),

      // Northeast quadrant
      glm::vec3(20.0f, 13.0f, -20.0f), glm::vec3(15.0f, 13.0f, -25.0f),
      glm::vec3(25.0f, 13.0f, -15.0f), glm::vec3(18.0f, 13.0f, -18.0f),

      // Southwest quadrant
      glm::vec3(-20.0f, 13.0f, 20.0f), glm::vec3(-15.0f, 13.0f, 25.0f),
      glm::vec3(-25.0f, 13.0f, 15.0f), glm::vec3(-18.0f, 13.0f, 18.0f),

      // Southeast quadrant
      glm::vec3(20.0f, 13.0f, 20.0f), glm::vec3(15.0f, 13.0f, 25.0f),
      glm::vec3(25.0f, 13.0f, 15.0f), glm::vec3(18.0f, 13.0f, 18.0f),

      // Central area
      glm::vec3(0.0f, 13.0f, 0.0f), glm::vec3(-8.0f, 13.0f, 8.0f),
      glm::vec3(8.0f, 13.0f, -8.0f), glm::vec3(-5.0f, 13.0f, 5.0f),
      glm::vec3(5.0f, 13.0f, -5.0f), glm::vec3(0.0f, 13.0f, 10.0f),
      glm::vec3(0.0f, 13.0f, -10.0f), glm::vec3(10.0f, 13.0f, 0.0f),
      glm::vec3(-10.0f, 13.0f, 0.0f)};

  for (const auto &pos : stalactitePositions) {
    auto stalactite = std::make_unique<Stalactite>(pos);
    objects.push_back(std::move(stalactite));
  }
}

void Level2::createGeysers() {
  // Create geyser vents in the floor - Many more for 60x60 space
  std::vector<glm::vec3> geyserPositions = {
      // Outer ring
      glm::vec3(-22.0f, 0.0f, -22.0f), glm::vec3(22.0f, 0.0f, -22.0f),
      glm::vec3(-22.0f, 0.0f, 22.0f), glm::vec3(22.0f, 0.0f, 22.0f),

      // Mid ring
      glm::vec3(-15.0f, 0.0f, -15.0f), glm::vec3(15.0f, 0.0f, -15.0f),
      glm::vec3(-15.0f, 0.0f, 15.0f), glm::vec3(15.0f, 0.0f, 15.0f),

      // Cardinal directions
      glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 0.0f, 20.0f),
      glm::vec3(-20.0f, 0.0f, 0.0f), glm::vec3(20.0f, 0.0f, 0.0f),

      // Inner area
      glm::vec3(-8.0f, 0.0f, -8.0f), glm::vec3(8.0f, 0.0f, -8.0f),
      glm::vec3(-8.0f, 0.0f, 8.0f), glm::vec3(8.0f, 0.0f, 8.0f),
      glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f)};

  for (const auto &pos : geyserPositions) {
    auto geyser = std::make_unique<Geyser>(pos);
    objects.push_back(std::move(geyser));
  }
}

void Level2::createCollectible() {
  // Glowing Gemstone - rough-cut red gem
  auto gemstone = std::make_unique<Collectible>(
      glm::vec3(0.0f, 2.0f, 12.0f), // Moved further back
      glm::vec3(0.9f, 0.2f, 0.2f)   // Red
  );
  objects.push_back(std::move(gemstone));

  // Rock base for the gem (embedded look)
  auto rockBase = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
  rockBase->transform.position = glm::vec3(0.0f, 1.5f, 12.0f);
  rockBase->transform.scale = glm::vec3(0.6f, 0.4f, 0.6f);
  rockBase->mesh.reset(Mesh::createCube(1.0f));
  rockBase->color = glm::vec3(0.3f, 0.25f, 0.2f); // Rock color
  rockBase->materialType = 3;                     // Rock texture
  rockBase->updateBoundingBox();
  objects.push_back(std::move(rockBase));
}

void Level2::createPedestal() {
  // Ancient stone pedestal - Moved further back
  auto ped = std::make_unique<GameObject>(GameObjectType::PEDESTAL);
  ped->transform.position = glm::vec3(0.0f, 0.5f, 15.0f);
  ped->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  ped->mesh.reset(Mesh::createCylinder(0.5f, 1.0f, 16));
  ped->color = glm::vec3(0.4f, 0.4f, 0.35f); // Stone grey
  ped->updateBoundingBox();
  ped->isTrigger = true;

  pedestal = ped.get();
  objects.push_back(std::move(ped));
}

void Level2::setupLighting() {
  // Multiple warm orange torches on walls - Adjusted for 60x60 size
  std::vector<glm::vec3> torchPositions = {
      // North wall
      glm::vec3(-25.0f, 6.0f, -28.0f), glm::vec3(0.0f, 6.0f, -28.0f),
      glm::vec3(25.0f, 6.0f, -28.0f),
      // South wall
      glm::vec3(-25.0f, 6.0f, 28.0f), glm::vec3(0.0f, 6.0f, 28.0f),
      glm::vec3(25.0f, 6.0f, 28.0f),
      // West wall
      glm::vec3(-28.0f, 6.0f, -20.0f), glm::vec3(-28.0f, 6.0f, 0.0f),
      glm::vec3(-28.0f, 6.0f, 20.0f),
      // East wall
      glm::vec3(28.0f, 6.0f, -20.0f), glm::vec3(28.0f, 6.0f, 0.0f),
      glm::vec3(28.0f, 6.0f, 20.0f)};

  for (const auto &pos : torchPositions) {
    // Create visual torch model
    auto torchModel = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
    torchModel->transform.position = pos;
    torchModel->transform.scale =
        glm::vec3(0.075f, 0.3f, 0.075f); // Reduced by 0.75
    torchModel->mesh.reset(
        Mesh::createCylinder(0.0375f, 0.3f, 8));     // Reduced by 0.75
    torchModel->color = glm::vec3(1.0f, 0.5f, 0.2f); // Glowing orange
    torchModel->materialType = 0;                    // No texture, just color
    torchModel->updateBoundingBox();
    objects.push_back(std::move(torchModel));

    // Create light
    Light torch;
    torch.position = pos;
    torch.color = glm::vec3(1.0f, 0.6f, 0.3f); // Warm orange
    torch.intensity = 4.0f;                    // Reduced further
    torch.flickerSpeed = 5.0f + (rand() % 100) / 100.0f;
    torch.flickerAmount = 0.3f;
    lights.push_back(torch);
  }

  ambientLight = glm::vec3(0.15f); // Dimmer ambient light
}

void Level2::update(float deltaTime, Player *player,
                    ParticleSystem *particles) {
  Level::update(deltaTime, player, particles);

  // Check stalactite triggers (proximity)
  for (auto &obj : objects) {
    if (obj->type == GameObjectType::STALACTITE && obj->isActive) {
      Stalactite *stal = dynamic_cast<Stalactite *>(obj.get());
      if (stal && !stal->isFalling) {
        // Check horizontal distance (XZ plane) since stalactite is high up
        glm::vec2 playerPosXZ(player->getPosition().x, player->getPosition().z);
        glm::vec2 stalPosXZ(stal->transform.position.x,
                            stal->transform.position.z);

        float dist = glm::length(playerPosXZ - stalPosXZ);

        // Trigger if player is under the stalactite (within 2 units
        // horizontally)
        if (dist < 2.0f) {
          stal->onTrigger();
        }
      }
    }
  }

  // Check geyser push
  for (auto &obj : objects) {
    if (obj->type == GameObjectType::GEYSER && obj->isActive) {
      Geyser *geyser = dynamic_cast<Geyser *>(obj.get());
      if (geyser && geyser->isErupting) {
        // Check if player is above the geyser (in the steam column)
        glm::vec3 playerPos = player->getPosition();
        glm::vec3 geyserPos = geyser->transform.position;

        // Check horizontal distance (XZ plane)
        glm::vec2 playerPosXZ(playerPos.x, playerPos.z);
        glm::vec2 geyserPosXZ(geyserPos.x, geyserPos.z);
        float horizontalDist = glm::length(playerPosXZ - geyserPosXZ);

        // Check if player is within geyser radius and above it
        float geyserRadius = 1.5f; // Much larger radius for bigger hitbox
        float steamHeight = 5.0f;  // Height of steam column

        bool inSteamColumn = (horizontalDist < geyserRadius) &&
                             (playerPos.y >= geyserPos.y) &&
                             (playerPos.y < geyserPos.y + steamHeight);

        if (inSteamColumn) {
          // Calculate pushback direction (away from geyser center)
          glm::vec2 horizontalDir = playerPosXZ - geyserPosXZ;

          if (horizontalDist > 0.01f) {
            horizontalDir = glm::normalize(horizontalDir);
          } else {
            horizontalDir = glm::vec2(1.0f, 0.0f); // Default direction
          }

          // Combined push force: upward + outward (way stronger)
          glm::vec3 totalPush = glm::vec3(
              horizontalDir.x * 20.0f, // Much stronger horizontal pushback
              35.0f,                   // Much stronger upward force
              horizontalDir.y * 20.0f  // Much stronger horizontal pushback
          );

          player->transform.position += totalPush * deltaTime;
        }

        // Emit steam particles
        // White/Grey color, moving up
        particles->emit(
            geyser->transform.position +
                glm::vec3(0.0f, 0.5f, 0.0f),   // Start slightly above vent
            glm::vec3(0.0f, 5.0f, 0.0f),       // Upward velocity
            glm::vec4(0.8f, 0.8f, 0.9f, 0.5f), // White-ish steam
            0.3f,                              // Larger particles
            1.0f,                              // Lifetime
            8                                  // More particles per frame
        );
      }
    }
  }

  // Check if gem was collected and player reached pedestal
  if (hasCollectible && pedestal) {
    if (Physics::checkSphereAABBCollision(player->collisionSphere,
                                          pedestal->boundingBox)) {
      if (!gemPlaced) {
        gemPlaced = true;

        // Add pulsing light to gemstone on pedestal
        Light gemLight;
        gemLight.position =
            pedestal->transform.position + glm::vec3(0.0f, 1.0f, 0.0f);
        gemLight.color = glm::vec3(0.9f, 0.2f, 0.2f);
        gemLight.intensity = 20.0f;
        gemLight.flickerSpeed = 3.0f;
        gemLight.flickerAmount = 0.5f;
        lights.push_back(gemLight);

        levelComplete = true;
      }
    }
  }
}

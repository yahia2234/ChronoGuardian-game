#include "Level2.h"
#include "AudioManager.h"

Level2::Level2()
    : pedestal(nullptr), gemCollectible(nullptr), gemPlaced(false),
      stalactiteHits(0), endingCutscene(false), cutsceneTimer(0.0f),
      gemOriginalPos(0.0f), gemShakeOffset(0.0f), pedestalRiseAmount(0.0f) {
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
  // Create 50 stalactites - distributed across 60x60 space
  std::vector<glm::vec3> stalactitePositions;

  // Generate 50 stalactites in a grid pattern with some randomness
  for (int i = 0; i < 50; i++) {
    float x = -25.0f + (i % 10) * 5.5f + (rand() % 100 - 50) / 50.0f;
    float z = -25.0f + (i / 10) * 11.0f + (rand() % 100 - 50) / 50.0f;
    stalactitePositions.push_back(glm::vec3(x, 13.0f, z));
  }

  for (const auto &pos : stalactitePositions) {
    auto stalactite = std::make_unique<Stalactite>(pos);
    // Make them much bigger - 2x scale
    stalactite->transform.scale = glm::vec3(0.8f, 4.0f, 0.8f); // Twice as big
    stalactite->updateBoundingBox();
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
  // Glowing Gemstone - placed far from pedestal in corner
  auto gemstone = std::make_unique<Collectible>(
      glm::vec3(-20.0f, 2.0f, -20.0f), // Far northwest corner
      glm::vec3(0.9f, 0.2f, 0.2f)      // Red
  );
  gemCollectible = gemstone.get();               // Store reference for cutscene
  gemOriginalPos = gemstone->transform.position; // Store original position
  objects.push_back(std::move(gemstone));

  // Rock base for the gem (embedded look)
  auto rockBase = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
  rockBase->transform.position = glm::vec3(-20.0f, 1.5f, -20.0f);
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
    torch.intensity = 5.0f;                    // Reduced further
    torch.flickerSpeed = 5.0f + (rand() % 100) / 100.0f;
    torch.flickerAmount = 0.3f;
    lights.push_back(torch);
  }

  ambientLight = glm::vec3(0.25f); // Dimmer ambient light
}

void Level2::update(float deltaTime, Player *player,
                    ParticleSystem *particles) {
  Level::update(deltaTime, player, particles);

  // Check stalactite triggers (proximity) and collisions
  for (auto &obj : objects) {
    if (obj->type == GameObjectType::STALACTITE && obj->isActive) {
      Stalactite *stal = dynamic_cast<Stalactite *>(obj.get());
      if (stal) {
        glm::vec2 playerPosXZ(player->getPosition().x, player->getPosition().z);
        glm::vec2 stalPosXZ(stal->transform.position.x,
                            stal->transform.position.z);
        float dist = glm::length(playerPosXZ - stalPosXZ);

        if (!stal->isFalling) {
          // Only trigger if player is BELOW the stalactite (not just nearby)
          // Check horizontal distance AND vertical position
          // Bigger trigger radius: 3.5 units
          if (dist < 3.5f) {
            // Player must be below the stalactite
            float playerY = player->getPosition().y;
            float stalY = stal->transform.position.y;

            // Only fall if player is below (stalactite is higher than player)
            if (stalY > playerY + 1.0f) { // At least 1 unit above player
              stal->onTrigger();
            }
          }
        } else {
          // Check collision with falling stalactite (bigger hitbox: 2.0 units)
          if (dist < 2.0f) {
            // Check if stalactite is at player height
            float heightDiff =
                std::abs(stal->transform.position.y - player->getPosition().y);
            if (heightDiff < 2.0f) {
              // Hit detected!
              stalactiteHits++;

              // Play impact sound
              AudioManager::getInstance().playSound(SoundEffect::OBSTACLE_HIT,
                                                    0.8f);

              // Strong pushback
              glm::vec3 pushDir =
                  glm::normalize(glm::vec3(playerPosXZ.x - stalPosXZ.x, 0.0f,
                                           playerPosXZ.y - stalPosXZ.y));
              player->transform.position += pushDir * 8.0f; // Strong pushback

              // Emit impact particles
              particles->emitExplosion(
                  stal->transform.position,
                  glm::vec3(0.6f, 0.4f, 0.3f), // Rock color
                  25);

              // Deactivate stalactite
              stal->isActive = false;

              // Check if player has been hit 3 times - reset to Level 1
              if (stalactiteHits >= 3) {
                shouldResetToLevel1 = true;
                return;
              }
            }
          }
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
  if (hasCollectible && pedestal && !endingCutscene) {
    if (Physics::checkSphereAABBCollision(player->collisionSphere,
                                          pedestal->boundingBox)) {
      if (!gemPlaced) {
        gemPlaced = true;
        endingCutscene = true;
        cutsceneTimer = 0.0f;

        // Disable player controls during cutscene
        player->controlsEnabled = false;

        // Make gem reappear on pedestal - reset its collected state
        if (gemCollectible) {
          auto collectible = static_cast<Collectible *>(gemCollectible);
          collectible->isCollected = false; // Un-collect it
          collectible->isActive = true;     // Make it active
          collectible->isTrigger = false;   // Don't trigger collection
          collectible->transform.position =
              pedestal->transform.position + glm::vec3(0.0f, 1.5f, 0.0f);
          collectible->transform.scale =
              glm::vec3(1.2f, 1.2f, 0.2f); // Make it visible
        }
      }
    }
  }

  // Handle ending cutscene animation
  if (endingCutscene) {
    cutsceneTimer += deltaTime;

    if (cutsceneTimer < 2.0f) {
      // Phase 1: Gem shakes and bobs for 2 seconds
      if (gemCollectible) {
        float shake = sin(cutsceneTimer * 30.0f) * 0.1f; // Fast shake
        float bob = sin(cutsceneTimer * 5.0f) * 0.3f;    // Slow bob
        gemCollectible->transform.position =
            pedestal->transform.position + glm::vec3(shake, 1.5f + bob, shake);

        // Add pulsing red light from the gem
        float pulseIntensity =
            10.0f + sin(cutsceneTimer * 8.0f) * 5.0f; // Pulse 5-15
        Light gemLight;
        gemLight.position = gemCollectible->transform.position;
        gemLight.color = glm::vec3(1.0f, 0.1f, 0.1f); // Bright red
        gemLight.intensity = pulseIntensity;
        gemLight.flickerSpeed = 0.0f;
        gemLight.flickerAmount = 0.0f;
        // Update or add light (replace last light if it exists)
        if (lights.size() > 15) {
          lights.back() = gemLight;
        } else {
          lights.push_back(gemLight);
        }
      }
    } else if (cutsceneTimer < 4.0f) {
      // Phase 2: Pedestal rises (2-4 seconds)
      float riseProgress = (cutsceneTimer - 2.0f) / 2.0f; // 0 to 1
      pedestalRiseAmount = riseProgress * 3.0f;           // Rise 3 units
      pedestal->transform.position.y = 0.5f + pedestalRiseAmount;

      // Gem rises with pedestal
      if (gemCollectible) {
        gemCollectible->transform.position =
            pedestal->transform.position + glm::vec3(0.0f, 1.5f, 0.0f);
      }

      // Intensifying pulsing light - gets brighter as pedestal rises
      float baseIntensity = 20.0f + riseProgress * 30.0f; // 20 to 50
      float pulse = sin(cutsceneTimer * 10.0f) * 10.0f;   // Faster pulse
      float totalIntensity = baseIntensity + pulse;

      Light gemLight;
      gemLight.position =
          pedestal->transform.position + glm::vec3(0.0f, 2.0f, 0.0f);
      gemLight.color = glm::vec3(1.0f, 0.05f, 0.05f); // Very bright red
      gemLight.intensity = totalIntensity;
      gemLight.flickerSpeed = 0.0f;
      gemLight.flickerAmount = 0.0f;

      // Update light
      if (lights.size() > 15) {
        lights.back() = gemLight;
      } else {
        lights.push_back(gemLight);
      }
    } else {
      // Phase 3: Fade to black and complete level
      levelComplete = true;
    }
  }
}

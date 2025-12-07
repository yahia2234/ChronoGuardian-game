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
  createSkeletons(); // Add skeletons in corners
  createCollectible();
  createPedestal();
  setupLighting();
  
  // Create health pickups in Level 2 (2 pickups in different areas)
  auto healthPickup1 = std::make_unique<HealthPickup>(glm::vec3(-15.0f, 1.5f, 10.0f));
  objects.push_back(std::move(healthPickup1));
  
  auto healthPickup2 = std::make_unique<HealthPickup>(glm::vec3(18.0f, 1.5f, -12.0f));
  objects.push_back(std::move(healthPickup2));
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
  walls.back()->materialType = 4; // Cave wall texture

  // South wall
  createWall(glm::vec3(0.0f, roomHeight / 2, -roomDepth / 2),
             glm::vec3(roomWidth, roomHeight, 1.0f), rockColor);
  walls.back()->materialType = 4; // Cave wall texture

  // East wall (irregular rough rock)
  createWall(glm::vec3(roomWidth / 2, roomHeight / 2, 0.0f),
             glm::vec3(1.0f, roomHeight, roomDepth), rockColor);
  walls.back()->materialType = 4; // Cave wall texture

  // West wall
  createWall(glm::vec3(-roomWidth / 2, roomHeight / 2, 0.0f),
             glm::vec3(1.0f, roomHeight, roomDepth), rockColor);
  walls.back()->materialType = 4; // Cave wall texture

  // Ceiling (rough cave rock)
  createWall(glm::vec3(0.0f, roomHeight, 0.0f),
             glm::vec3(roomWidth, 1.0f, roomDepth), rockColor);
  walls.back()->materialType = 4; // Cave wall texture

  // Cave floor with visible rocky texture - using cube mesh for visibility
  glm::vec3 caveFloorColor(0.85f, 0.75f, 0.60f); // Very light brown for visibility
  createWall(glm::vec3(0.0f, -0.5f, 0.0f),  // Centered at Y=-0.5 so top is at Y=0
             glm::vec3(roomWidth, 1.0f, roomDepth), caveFloorColor);
  walls.back()->materialType = 4; // Cave texture for ground
  
  // Create a winding muddy path using oriented rectangular segments
  glm::vec3 mudColor(0.50f, 0.38f, 0.28f); // Visible muddy brown
  
  // Define path waypoints that curve around the vents (serpentine path)
  std::vector<glm::vec2> pathPoints = {
    {0.0f, -28.0f},    // Start at south
    {-3.0f, -25.0f},   // Smooth transition
    {-6.0f, -22.0f},   // Curve left around vent at (0, -20)
    {-8.0f, -18.0f},   // Smooth curve
    {-10.0f, -15.0f},  // Continue curving  
    {-8.0f, -11.0f},   // Smooth transition
    {-5.0f, -8.0f},    // Curve back right around vent at (-8, -8)
    {0.0f, -5.0f},     // Smooth curve
    {4.0f, -3.0f},     // Pass between center vents
    {6.0f, 0.0f},      // Smooth curve
    {8.0f, 3.0f},      // Curve right around vent at (0, 5)
    {7.0f, 7.0f},      // Smooth curve
    {5.0f, 10.0f},     // Continue curving
    {1.0f, 13.0f},     // Smooth curve
    {-4.0f, 15.0f},    // Curve left around vent at (8, 8)
    {-6.0f, 18.0f},    // Smooth curve
    {-8.0f, 20.0f},    // Continue curving
    {-5.0f, 24.0f},    // Smooth transition
    {0.0f, 28.0f}      // End at north
  };
  
  // Create connected rectangular segments between waypoints
  float pathWidth = 4.0f; // Width of the muddy path
  
  for (size_t i = 0; i < pathPoints.size() - 1; i++) {
    glm::vec2 start = pathPoints[i];
    glm::vec2 end = pathPoints[i + 1];
    
    // Calculate segment properties
    glm::vec2 direction = end - start;
    float length = glm::length(direction);
    glm::vec2 midpoint = (start + end) * 0.5f;
    
    // Calculate rotation angle (around Y axis)
    float angle = atan2(direction.x, direction.y);
    
    // Slight color variation for natural look
    glm::vec3 patchColor = mudColor + glm::vec3((rand() % 8 - 4) / 100.0f, 
                                                 (rand() % 6 - 3) / 100.0f, 
                                                 (rand() % 4 - 2) / 100.0f);
    
    // Create oriented rectangular segment
    auto mudSegment = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
    mudSegment->transform.position = glm::vec3(midpoint.x, 0.02f, midpoint.y);
    mudSegment->transform.scale = glm::vec3(pathWidth, 0.08f, length + 0.5f);
    mudSegment->transform.rotate(angle, glm::vec3(0, 1, 0)); // Rotate to align with path
    mudSegment->mesh.reset(Mesh::createCube(1.0f));
    mudSegment->color = patchColor;
    mudSegment->materialType = 5;
    mudSegment->updateBoundingBox();
    walls.push_back(std::move(mudSegment));
  }
  
  // Add circular disc caps at each waypoint to smooth the angle transitions
  for (size_t i = 0; i < pathPoints.size(); i++) {
    glm::vec2 pos = pathPoints[i];
    
    // Slight color variation
    glm::vec3 capColor = mudColor + glm::vec3((rand() % 6 - 3) / 100.0f,
                                               (rand() % 4 - 2) / 100.0f,
                                               (rand() % 3 - 1) / 100.0f);
    
    // Create circular cap using cylinder (flat disc)
    auto cap = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
    cap->transform.position = glm::vec3(pos.x, 0.025f, pos.y);
    cap->transform.scale = glm::vec3(pathWidth * 0.55f, 0.06f, pathWidth * 0.55f); // Slightly larger than path width
    cap->mesh.reset(Mesh::createCylinder(1.0f, 1.0f, 24)); // 24-segment cylinder for smooth circle
    cap->color = capColor;
    cap->materialType = 5;
    cap->updateBoundingBox();
    walls.push_back(std::move(cap));
  }
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
    
    // Add a complete tight circle of rocks around each vent using rock model
    int numRocks = 20 + (rand() % 5); // 20-24 rocks for complete tight circle
    for (int r = 0; r < numRocks; r++) {
      // Evenly spaced around the circle
      float angle = (float)r / numRocks * 2.0f * 3.14159f;
      float distance = 2.2f + (rand() % 30) / 100.0f; // 2.2-2.5 units from center
      
      float rockX = pos.x + cos(angle) * distance;
      float rockZ = pos.z + sin(angle) * distance;
      
      // Rock scale
      float rockScale = 0.15f + (rand() % 10) / 100.0f; // 0.15-0.25 scale
      
      auto rock = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
      rock->transform.position = glm::vec3(rockX, 0.0f, rockZ);
      rock->transform.scale = glm::vec3(rockScale);
      // Random rotation for variety
      rock->transform.rotate((rand() % 360) * 3.14159f / 180.0f, glm::vec3(0, 1, 0));
      rock->loadModel("assets/models/random_rock.glb");
      rock->color = glm::vec3(0.55f, 0.48f, 0.40f); // Light brown-gray
      rock->materialType = 4; // Cave rock texture
      rock->updateBoundingBox();
      objects.push_back(std::move(rock));
    }
  }
}

void Level2::createSkeletons() {
  // Add skeleton models in each corner of the cave
  std::vector<std::pair<glm::vec3, float>> cornerPositions = {
    {{-26.0f, 0.0f, -26.0f}, 45.0f},   // Southwest corner, facing NE
    {{26.0f, 0.0f, -26.0f}, 135.0f},   // Southeast corner, facing NW
    {{-26.0f, 0.0f, 26.0f}, -45.0f},   // Northwest corner, facing SE
    {{26.0f, 0.0f, 26.0f}, -135.0f}    // Northeast corner, facing SW
  };
  
  for (const auto& [pos, rotation] : cornerPositions) {
    auto skeleton = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
    skeleton->transform.position = pos;
    skeleton->transform.scale = glm::vec3(0.1f); // Small skeleton size
    skeleton->transform.rotate(glm::radians(rotation), glm::vec3(0, 1, 0));
    skeleton->loadModel("assets/models/human_skeleton_download_free.glb");
    skeleton->color = glm::vec3(0.9f, 0.85f, 0.75f); // Bone white color
    skeleton->isActive = true;
    skeleton->updateBoundingBox();
    objects.push_back(std::move(skeleton));
  }
}

void Level2::createCollectible() {
  // Glowing Gemstone - placed far from pedestal in corner
  auto gemstone = std::make_unique<Collectible>(
      glm::vec3(-20.0f, 2.0f, -20.0f), // Far northwest corner
      glm::vec3(0.9f, 0.2f, 0.2f)      // Red
  );
  
  // Load new model
  gemstone->loadModel("assets/models/crystal_pendant_updated_2022.glb");
  gemstone->transform.scale = glm::vec3(0.5f); // Uniform scale for model
  
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
  
  // Load new model
  ped->loadModel("assets/models/ancient_greek_column_remains.glb");
  // Scale it so it's bigger in the vertical length (0.2, 0.6, 0.2)
  ped->transform.scale = glm::vec3(0.2f, 0.6f, 0.2f); 
  
  ped->color = glm::vec3(0.4f, 0.4f, 0.35f); // Stone grey (tint for model if supported)
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
    
    // Load new model
    torchModel->loadModel("assets/models/medieval_torch.glb");
    torchModel->transform.scale = glm::vec3(0.15f); // Much smaller scale
    
    // Rotate to stick out of wall and tilt slightly up
    // Also offset position slightly to not be buried in wall
    glm::vec3 adjustedPos = pos;
    float offset = 0.2f; // Distance from wall center
    float tiltAngle = glm::radians(-15.0f); // Tilt up slightly

    // Adjusted rotations to align flat side with wall (rotated 90 degrees from previous)
    if (pos.z < -27.0f) { // North wall
        adjustedPos.z += offset;
        torchModel->transform.position = adjustedPos;
        torchModel->transform.rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)); 
        torchModel->transform.rotate(tiltAngle, glm::vec3(1, 0, 0)); // Tilt up
    } else if (pos.z > 27.0f) { // South wall
        adjustedPos.z -= offset;
        torchModel->transform.position = adjustedPos;
        torchModel->transform.rotate(glm::radians(-90.0f), glm::vec3(0, 1, 0));
        torchModel->transform.rotate(tiltAngle, glm::vec3(1, 0, 0)); // Tilt up
    } else if (pos.x < -27.0f) { // West wall
        adjustedPos.x += offset;
        torchModel->transform.position = adjustedPos;
        torchModel->transform.rotate(glm::radians(0.0f), glm::vec3(0, 1, 0));
        torchModel->transform.rotate(tiltAngle, glm::vec3(1, 0, 0)); // Tilt up
    } else if (pos.x > 27.0f) { // East wall
        adjustedPos.x -= offset;
        torchModel->transform.position = adjustedPos;
        torchModel->transform.rotate(glm::radians(180.0f), glm::vec3(0, 1, 0));
        torchModel->transform.rotate(tiltAngle, glm::vec3(1, 0, 0)); // Tilt up
    }
    
    torchModel->color = glm::vec3(1.0f, 0.5f, 0.2f); // Glowing orange tint
    torchModel->updateBoundingBox();
    objects.push_back(std::move(torchModel));

    // Create light
    Light torch;
    torch.position = adjustedPos + glm::vec3(0.0f, 0.3f, 0.0f); // Light slightly above torch
    torch.color = glm::vec3(1.0f, 0.5f, 0.2f); // Warm orange
    torch.baseIntensity = 1.5f;                // Base intensity
    torch.intensity = torch.baseIntensity;
    torch.flickerSpeed = 1.0f + (rand() % 100) / 100.0f; // Much slower flicker (1.0 - 2.0)
    torch.flickerAmount = 0.8f; // Large flicker range
    lights.push_back(torch);
  }

  ambientLight = glm::vec3(0.15f); // Darker ambient light for cave atmosphere
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
              
              // Take heart damage
              player->takeDamage();

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
          
          // Take heart damage once per geyser contact (using flash as cooldown)
          if (player->damageFlashIntensity < 0.1f) {
            player->takeDamage();
          }
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
              glm::vec3(0.5f); // Uniform scale for model
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

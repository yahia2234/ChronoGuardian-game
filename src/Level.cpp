#include "Level.h"
#include "Shader.h"
#include <cmath>
#include <iostream>

Level::Level()
    : ambientLight(0.2f), playerStartPosition(0.0f, 1.0f, 0.0f),
      levelComplete(false), hasCollectible(false), shouldRestart(false),
      shouldResetToLevel1(false) {}

void Level::update(float deltaTime, Player *player, ParticleSystem *particles) {
  // Update all game objects
  for (auto &obj : objects) {
    if (obj->isActive) {
      obj->update(deltaTime);
    }
  }

  // Update lights (flickering)
  // Update lights (flickering)
  for (auto &light : lights) {
    if (light.flickerSpeed > 0.0f) {
      light.flickerOffset += light.flickerSpeed * deltaTime;
      // Flicker around baseIntensity
      // sin varies from -1 to 1, so intensity varies from base - amount to base
      // + amount
      float flicker = sin(light.flickerOffset) * light.flickerAmount;
      light.intensity = std::max(0.1f, light.baseIntensity + flicker);
    }
  }

  // Check collisions and triggers
  checkCollisions(player, particles);
  checkTriggers(player);
}

void Level::draw(Shader *shader) {
  // Draw walls
  for (const auto &wall : walls) {
    if (wall->isActive) {
      wall->draw(shader);
    }
  }

  // Draw objects
  for (const auto &obj : objects) {
    if (obj->isActive) {
      obj->draw(shader);
    }
  }

  // Draw light fixtures (glowing orbs) - rendered last with additive-like
  // effect
  for (const auto &fixture : lightFixtures) {
    if (fixture->isActive) {
      fixture->draw(shader);
    }
  }
}

void Level::drawLights(Shader *shader) {
  shader->setVec3("ambientLight", ambientLight);
  // Increased limit to 32 to match shader update
  shader->setInt("numLights", std::min((int)lights.size(), 32));

  for (size_t i = 0; i < lights.size() && i < 32; i++) {
    std::string base = "lights[" + std::to_string(i) + "]";
    shader->setVec3(base + ".position", lights[i].position);
    shader->setVec3(base + ".color", lights[i].color);
    shader->setFloat(base + ".intensity", lights[i].intensity);
  }
}

void Level::checkCollisions(Player *player, ParticleSystem *particles) {
  // Check wall collisions - ACTUALLY PREVENT PENETRATION
  for (const auto &wall : walls) {
    if (!wall->isActive)
      continue;

    if (Physics::checkSphereAABBCollision(player->collisionSphere,
                                          wall->boundingBox)) {
      // Calculate penetration and push player out
      glm::vec3 playerPos = player->getPosition();
      glm::vec3 wallCenter = wall->boundingBox.getCenter();
      glm::vec3 wallSize = wall->boundingBox.max - wall->boundingBox.min;

      // Find closest point on box to sphere
      glm::vec3 closestPoint =
          glm::clamp(playerPos, wall->boundingBox.min, wall->boundingBox.max);
      glm::vec3 normal = playerPos - closestPoint;
      float distance = glm::length(normal);

      if (distance < player->collisionSphere.radius) {
        // Push player out
        if (distance > 0.001f) {
          normal = glm::normalize(normal);
        } else {
          // Player is exactly at closest point, use direction from wall center
          normal = glm::normalize(playerPos - wallCenter);
        }

        float penetration = player->collisionSphere.radius - distance;
        player->transform.position +=
            normal *
            (penetration + 0.01f); // Small epsilon to ensure separation
        player->collisionSphere.center = player->transform.position;

        // Play collision sound and emit particles
        player->onWallCollision(normal, particles);
      }
    }
  }

  // Check obstacle collisions
  for (auto &obj : objects) {
    if (!obj->isActive || obj->isTrigger)
      continue;

    bool collision = false;
    if (obj->useSphereCollision) {
      collision = Physics::checkSphereCollision(player->collisionSphere,
                                                obj->boundingSphere);
    } else {
      collision = Physics::checkSphereAABBCollision(player->collisionSphere,
                                                    obj->boundingBox);
    }

    if (collision) {
      if (obj->type == GameObjectType::PENDULUM) {
        glm::vec3 knockback =
            glm::normalize(player->getPosition() - obj->transform.position);
        player->onObstacleHit(knockback, particles);
      } else if (obj->type == GameObjectType::STALACTITE) {
        // If stalactite hits player while falling, it's a hazard
        Stalactite *stal = dynamic_cast<Stalactite *>(obj.get());
        if (stal && stal->isFalling) {
          // Apply pushback/knockback
          glm::vec3 knockback =
              glm::normalize(player->getPosition() - stal->transform.position);
          // Add some upward force too
          knockback.y = 0.5f;
          knockback = glm::normalize(knockback);

          player->onObstacleHit(knockback, particles);
        }
      }
    }
  }
}

void Level::checkTriggers(Player *player) {
  // Check triggers
  for (auto &obj : objects) {
    if (obj->isActive && obj->isTrigger) {
      // Special handling for crumbling tiles - check if player is standing on
      // top
      if (obj->type == GameObjectType::CRUMBLING_TILE) {
        glm::vec3 playerPos = player->getPosition();
        glm::vec3 tilePos = obj->transform.position;
        glm::vec3 tileScale = obj->transform.scale;

        // Check if player is above the tile (within reasonable height)
        float heightDiff = playerPos.y - tilePos.y;
        bool isAboveTile = (heightDiff > 0.0f && heightDiff < 2.0f);

        // Check horizontal overlap (is player's XZ position over the tile?)
        bool xOverlap = std::abs(playerPos.x - tilePos.x) <
                        (tileScale.x / 2.0f + player->collisionSphere.radius);
        bool zOverlap = std::abs(playerPos.z - tilePos.z) <
                        (tileScale.z / 2.0f + player->collisionSphere.radius);

        if (isAboveTile && xOverlap && zOverlap) {
          obj->onTrigger();
        }
      } else if (obj->useSphereCollision) {
        // Enhanced debug logging for collectibles
        if (obj->type == GameObjectType::COLLECTIBLE) {
          auto collectible = static_cast<Collectible *>(obj.get());
          if (!collectible->isCollected) {
            float distance = glm::length(player->collisionSphere.center -
                                         obj->boundingSphere.center);
            float combinedRadius =
                player->collisionSphere.radius + obj->boundingSphere.radius;

            // Debug output every frame when player is near gem
          }
        }

        if (Physics::checkSphereCollision(player->collisionSphere,
                                          obj->boundingSphere)) {
          obj->onTrigger();
          if (obj->type == GameObjectType::COLLECTIBLE) {
            auto collectible = static_cast<Collectible *>(obj.get());
            if (!collectible->isCollected) {
              collectible->collect();
              hasCollectible = true;
            }
          } else if (obj->type == GameObjectType::HEALTH_PICKUP) {
            auto healthPickup = static_cast<HealthPickup *>(obj.get());
            if (!healthPickup->isCollected) {
              healthPickup->collect();
              player->addHeart();
            }
          }
        }
      } else {
        if (Physics::checkSphereAABBCollision(player->collisionSphere,
                                              obj->boundingBox)) {
          obj->onTrigger();
        }
      }
    }
  }
}

void Level::checkCameraCollision(glm::vec3 &cameraPos,
                                 const glm::vec3 &targetPos) {
  // Reset all walls to opaque first
  for (auto &wall : walls) {
    wall->transparency = 1.0f;
  }

  glm::vec3 rayDir = cameraPos - targetPos;
  float maxDist = glm::length(rayDir);
  rayDir = glm::normalize(rayDir);

  // Check for walls between camera and player
  for (auto &wall : walls) {
    float t;
    if (Physics::rayIntersectAABB(targetPos, rayDir, wall->boundingBox, t)) {
      // If wall is between player and camera (with some buffer)
      if (t > 0.5f && t < maxDist - 0.5f) {
        // Make it transparent!
        wall->transparency = 0.3f;
      }
    }
  }

  // No longer moving the camera - we just make walls see-through
}

void Level::createWall(const glm::vec3 &position, const glm::vec3 &scale,
                       const glm::vec3 &color, int materialType) {
  auto wall = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
  wall->transform.position = position;
  wall->transform.scale = scale;
  wall->mesh.reset(Mesh::createCube(1.0f));
  wall->color = color;
  wall->materialType = materialType;
  wall->updateBoundingBox();
  walls.push_back(std::move(wall));
}

void Level::createFloor(const glm::vec3 &position, const glm::vec3 &scale,
                        const glm::vec3 &color) {
  auto floor = std::make_unique<GameObject>(GameObjectType::STATIC_WALL);
  floor->transform.position = position;
  floor->transform.scale = scale;
  floor->mesh.reset(
      Mesh::createPlane(1.0f, 1.0f)); // Use unit plane, scale via transform
  floor->color = color;
  floor->updateBoundingBox();
  walls.push_back(std::move(floor));
}

void Level::loadLightFixtureModel() {
  // Load the fractured orb model for light fixtures
  try {
    lightFixtureModel =
        std::make_unique<Model>("assets/models/fractured_orb.glb");
    std::cout << "Loaded fractured orb model for light fixtures" << std::endl;
  } catch (...) {
    std::cout
        << "Warning: Could not load fractured_orb.glb, using fallback spheres"
        << std::endl;
    lightFixtureModel = nullptr;
  }
}

void Level::drawLightFixtureModels(Shader *shader) {
  // Draw the fractured orb model at each light fixture position
  if (!lightFixtureModel || lightFixtureModel->meshes.empty()) {
    return;
  }

  // Set emissive mode for the orb - BOOSTED BRIGHTNESS
  shader->setFloat("emissive", 1.5f); // Brighter than normal (1.0)
  shader->setVec3("objectColor",
                  glm::vec3(1.0f, 0.95f, 0.85f)); // Warm white glow
  shader->setBool("useTexture", false);

  glDisable(GL_CULL_FACE); // Model might be inside-out

  for (const auto &transform : lightFixtureTransforms) {
    shader->setMat4("model", transform);

    // Calculate normal matrix
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    shader->setMat3("normalMatrix", normalMatrix);

    lightFixtureModel->draw(shader);
  }

  glEnable(GL_CULL_FACE);

  // Reset emissive
  shader->setFloat("emissive", 0.0f);
}

void Level::createLightFixture(const glm::vec3 &position,
                               const glm::vec3 &lightColor, float scale) {
  // Load model if not already loaded
  if (!lightFixtureModel) {
    loadLightFixtureModel();
  }

  // Raise the fixture slightly as requested
  glm::vec3 adjustedPos = position + glm::vec3(0.0f, 0.8f, 0.0f);

  // Create transform matrix for this light fixture
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, adjustedPos);
  transform = glm::scale(
      transform,
      glm::vec3(scale * 2.0f)); // Smaller scale (0.8x of previous 2.5)

  // Add to transforms list for drawing
  lightFixtureTransforms.push_back(transform);

  // Create chain/rod to ceiling (not emissive - just decorative)
  auto rod = std::make_unique<GameObject>(GameObjectType::COLLECTIBLE);
  rod->transform.position =
      adjustedPos +
      glm::vec3(0.0f, scale * 3.5f, 0.0f); // Adjusted for new scale
  rod->transform.scale =
      glm::vec3(0.12f, scale * 4.0f, 0.12f); // Thinner and shorter
  rod->mesh.reset(Mesh::createCube(1.0f));
  rod->color = glm::vec3(0.4f, 0.35f, 0.25f); // Bronze color
  rod->isActive = true;
  rod->isTrigger = false;
  rod->updateBoundingBox();
  lightFixtures.push_back(std::move(rod));

  // Create small mounting bracket
  auto mount = std::make_unique<GameObject>(GameObjectType::COLLECTIBLE);
  mount->transform.position = adjustedPos + glm::vec3(0.0f, scale * 1.2f, 0.0f);
  mount->transform.scale = glm::vec3(scale * 0.4f, scale * 0.25f, scale * 0.4f);
  mount->mesh.reset(Mesh::createSphere(1.0f, 8, 8));
  mount->color = glm::vec3(0.5f, 0.4f, 0.3f); // Bronze color
  mount->isActive = true;
  mount->isTrigger = false;
  mount->updateBoundingBox();
  lightFixtures.push_back(std::move(mount));
}

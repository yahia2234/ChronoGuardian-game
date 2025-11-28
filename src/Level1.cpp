#include "Level1.h"
#include <iostream>

Level1::Level1() : forceFieldDoor(nullptr), mainGem(nullptr), shardsCollected(0), mainGemActive(false) {
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
    
    // Main bright light
    Light mainLight;
    mainLight.position = glm::vec3(0.0f, 18.0f, 0.0f);
    mainLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    mainLight.intensity = 1.2f;
    lights.push_back(mainLight);
    
    // Corner lights for 60x60 area
    float d = 25.0f;
    Light corner1; corner1.position = glm::vec3(d, 10.0f, d); corner1.color = glm::vec3(1.0f, 0.9f, 0.8f); corner1.intensity = 25.0f; lights.push_back(corner1);
    Light corner2; corner2.position = glm::vec3(-d, 10.0f, -d); corner2.color = glm::vec3(0.8f, 0.9f, 1.0f); corner2.intensity = 25.0f; lights.push_back(corner2);
    Light corner3; corner3.position = glm::vec3(d, 10.0f, -d); corner3.color = glm::vec3(1.0f, 0.8f, 0.8f); corner3.intensity = 25.0f; lights.push_back(corner3);
    Light corner4; corner4.position = glm::vec3(-d, 10.0f, d); corner4.color = glm::vec3(0.8f, 1.0f, 0.8f); corner4.intensity = 25.0f; lights.push_back(corner4);
    
    ambientLight = glm::vec3(0.4f);
}

void Level1::createChamber() {
    // EXPANDED ROOM: 60x60
    float roomWidth = 60.0f;
    float roomHeight = 20.0f;
    float roomDepth = 60.0f;
    
    // Outer walls - High definition (Darker stone)
    glm::vec3 outerWallColor(0.4f, 0.4f, 0.45f);
    glm::vec3 borderColor(0.3f, 0.3f, 0.35f); // Darker border
    
    // Main Walls
    createWall(glm::vec3(0.0f, roomHeight/2, roomDepth/2), glm::vec3(roomWidth, roomHeight, 1.0f), outerWallColor); // Back
    createWall(glm::vec3(0.0f, roomHeight/2, -roomDepth/2), glm::vec3(roomWidth, roomHeight, 1.0f), outerWallColor); // Front
    createWall(glm::vec3(roomWidth/2, roomHeight/2, 0.0f), glm::vec3(1.0f, roomHeight, roomDepth), outerWallColor); // Right
    createWall(glm::vec3(-roomWidth/2, roomHeight/2, 0.0f), glm::vec3(1.0f, roomHeight, roomDepth), outerWallColor); // Left
    
    // Borders/Pillars for definition
    float pSize = 3.0f;
    // Corners
    createWall(glm::vec3(-roomWidth/2 + 1.5f, roomHeight/2, -roomDepth/2 + 1.5f), glm::vec3(pSize, roomHeight, pSize), borderColor);
    createWall(glm::vec3(roomWidth/2 - 1.5f, roomHeight/2, -roomDepth/2 + 1.5f), glm::vec3(pSize, roomHeight, pSize), borderColor);
    createWall(glm::vec3(-roomWidth/2 + 1.5f, roomHeight/2, roomDepth/2 - 1.5f), glm::vec3(pSize, roomHeight, pSize), borderColor);
    createWall(glm::vec3(roomWidth/2 - 1.5f, roomHeight/2, roomDepth/2 - 1.5f), glm::vec3(pSize, roomHeight, pSize), borderColor);
    
    // Ceiling
    createWall(glm::vec3(0.0f, roomHeight, 0.0f), glm::vec3(roomWidth, 1.0f, roomDepth), outerWallColor);
    
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
    createWall(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(roomWidth, 0.5f, roomDepth), outerWallColor);
}

void Level1::createMazeWalls() {
    // MIDDLE WALLS - BRICK TEXTURE
    glm::vec3 brickColor(0.7f, 0.3f, 0.2f); // Reddish brick
    
    // Central Fortress
    createWall(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(8.0f, 8.0f, 8.0f), brickColor, 1); 
    
    // Maze Segments - Creating distinct zones
    // Zone 1: Top Left (Spawn Area) - Open but guarded
    createWall(glm::vec3(-15.0f, 4.0f, -10.0f), glm::vec3(1.0f, 8.0f, 20.0f), brickColor, 1);
    
    // Zone 2: Top Right - Narrow corridors
    createWall(glm::vec3(15.0f, 4.0f, -10.0f), glm::vec3(1.0f, 8.0f, 20.0f), brickColor, 1);
    createWall(glm::vec3(22.0f, 4.0f, -5.0f), glm::vec3(14.0f, 8.0f, 1.0f), brickColor, 1);
    
    // Zone 3: Bottom Area - Platforming
    createWall(glm::vec3(0.0f, 4.0f, 15.0f), glm::vec3(30.0f, 8.0f, 1.0f), brickColor, 1);
    
    // Platforms/Obstacles (Wooden Crates)
    glm::vec3 crateColor(0.6f, 0.4f, 0.25f); // Brown Wood
    
    // High platforms for coins (lowered to be reachable)
    createWall(glm::vec3(-25.0f, 2.0f, 0.0f), glm::vec3(3.0f, 4.0f, 3.0f), crateColor);
    createWall(glm::vec3(25.0f, 2.0f, 0.0f), glm::vec3(3.0f, 4.0f, 3.0f), crateColor);
    createWall(glm::vec3(0.0f, 2.5f, -20.0f), glm::vec3(4.0f, 5.0f, 4.0f), crateColor);
    
    // Steps
    createWall(glm::vec3(-20.0f, 1.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), crateColor);
    createWall(glm::vec3(20.0f, 1.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), crateColor);
}

void Level1::createPendulums() {
    // COMPLEX SWINGING WALLS
    
    // 1. The Gauntlet (Main path to Gem)
    for (int i = 0; i < 6; i++) {
        float zPos = 5.0f + (i * 3.0f);
        auto p = std::make_unique<Pendulum>(glm::vec3(0.0f, 8.0f, zPos), 6.0f);
        p->swingSpeed = 2.0f + (i % 3) * 1.0f;
        p->swingAngle = i * 1.0f;
        // Make them look like swinging walls
        p->transform.scale = glm::vec3(6.0f, 4.0f, 0.5f); 
        p->updateBoundingBox(); // Important for collision
        objects.push_back(std::move(p));
    }
    
    // 2. Side Guards (Guarding Coins)
    // Left Zone
    for (int i = 0; i < 3; i++) {
        auto p = std::make_unique<Pendulum>(glm::vec3(-20.0f, 8.0f, -5.0f + i*5.0f), 5.0f);
        p->swingSpeed = 3.0f;
        objects.push_back(std::move(p));
    }
    
    // Right Zone
    for (int i = 0; i < 3; i++) {
        auto p = std::make_unique<Pendulum>(glm::vec3(20.0f, 8.0f, -5.0f + i*5.0f), 5.0f);
        p->swingSpeed = 3.0f;
        p->swingAngle = 3.14f;
        objects.push_back(std::move(p));
    }
}

void Level1::createCrumblingTiles() {
    // Bridge to the Exit (Door at z=28)
    for (int z = 20; z < 26; z+=2) {
        auto tile = std::make_unique<CrumblingTile>(glm::vec3(0.0f, 0.1f, (float)z));
        tile->fallTimer = 0.6f; // Fast fall
        objects.push_back(std::move(tile));
    }
    
    // Trap tiles near coins
    objects.push_back(std::make_unique<CrumblingTile>(glm::vec3(-25.0f, 6.1f, 0.0f))); // On top of platform!
    objects.push_back(std::make_unique<CrumblingTile>(glm::vec3(25.0f, 6.1f, 0.0f)));
}

void Level1::createCollectible() {
    // 6 COINS (Yellow, Spinning)
    glm::vec3 coinColor(1.0f, 0.8f, 0.0f); // Gold
    
    // 1. Spawn Area (Easy)
    auto c1 = std::make_unique<Collectible>(glm::vec3(-20.0f, 2.0f, -20.0f), coinColor);
    c1->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f); c1->rotationSpeed = 6.0f;
    objects.push_back(std::move(c1));
    
    // 2. Opposite Corner (Easy)
    auto c2 = std::make_unique<Collectible>(glm::vec3(20.0f, 2.0f, -20.0f), coinColor);
    c2->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f); c2->rotationSpeed = 6.0f;
    objects.push_back(std::move(c2));
    
    // 3. High Platform Left (Medium)
    auto c3 = std::make_unique<Collectible>(glm::vec3(-25.0f, 5.0f, 0.0f), coinColor);
    c3->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f); c3->rotationSpeed = 6.0f;
    objects.push_back(std::move(c3));
    
    // 4. High Platform Right (Medium)
    auto c4 = std::make_unique<Collectible>(glm::vec3(25.0f, 5.0f, 0.0f), coinColor);
    c4->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f); c4->rotationSpeed = 6.0f;
    objects.push_back(std::move(c4));
    
    // 5. Behind Gauntlet (Hard)
    auto c5 = std::make_unique<Collectible>(glm::vec3(0.0f, 2.0f, 10.0f), coinColor);
    c5->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f); c5->rotationSpeed = 6.0f;
    objects.push_back(std::move(c5));
    
    // 6. Top of Central Fortress (Hard)
    auto c6 = std::make_unique<Collectible>(glm::vec3(0.0f, 6.5f, -20.0f), coinColor);
    c6->transform.scale = glm::vec3(0.8f, 0.8f, 0.1f); c6->rotationSpeed = 6.0f;
    objects.push_back(std::move(c6));
    
    // MAIN GEM (Blue, Slow Shrink)
    // Spawns in center after coins
    auto gem = std::make_unique<Collectible>(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.5f, 1.0f));
    gem->transform.scale = glm::vec3(1.5f);
    gem->rotationSpeed = 1.5f;
    gem->shrinkSpeed = 0.5f;
    gem->isActive = false;
    mainGem = gem.get();
    objects.push_back(std::move(gem));
}

void Level1::createForceFieldDoor() {
    // Create stone archway frame
    glm::vec3 frameColor(0.5f, 0.5f, 0.55f); // Stone color
    
    // Door moved to z=28.0 (Room depth is 60, so wall is at 30. 28 gives space)
    float doorZ = 28.0f;
    
    // Left pillar
    createWall(glm::vec3(-1.5f, 2.5f, doorZ), glm::vec3(0.5f, 5.0f, 0.5f), frameColor);
    // Right pillar
    createWall(glm::vec3(1.5f, 2.5f, doorZ), glm::vec3(0.5f, 5.0f, 0.5f), frameColor);
    // Top arch
    createWall(glm::vec3(0.0f, 5.25f, doorZ), glm::vec3(3.5f, 0.5f, 0.5f), frameColor);

    // The force field itself
    auto door = std::make_unique<GameObject>(GameObjectType::DOOR);
    door->transform.position = glm::vec3(0.0f, 2.5f, doorZ);
    door->transform.scale = glm::vec3(2.5f, 5.0f, 0.1f); // Slightly thinner than frame
    door->mesh.reset(Mesh::createCube(1.0f));
    door->color = glm::vec3(0.2f, 0.6f, 1.0f); // Glowing blue force field
    door->transparency = 0.2f; // More transparent to show it's open
    door->isActive = false; // ALWAYS OPEN
    door->updateBoundingBox();
    
    forceFieldDoor = door.get();
    walls.push_back(std::move(door));
}



void Level1::update(float deltaTime, Player* player, ParticleSystem* particles) {
    // Custom update logic to track shards
    for (auto& obj : objects) {
        if (!obj->isActive || !obj->isTrigger) continue;
        
        if (obj->type == GameObjectType::COLLECTIBLE) {
            if (Physics::checkSphereCollision(player->collisionSphere, obj->boundingSphere)) {
                auto collectible = static_cast<Collectible*>(obj.get());
                if (!collectible->isCollected) {
                    collectible->collect();
                    
                    if (collectible == mainGem) {
                        hasCollectible = true;
                        std::cout << "MAIN GEM COLLECTED!" << std::endl;
                    } else {
                        shardsCollected++;
                        std::cout << "Coin collected! " << shardsCollected << "/" << totalShards << std::endl;
                        
                        // Emit Yellow Flare
                        particles->emitExplosion(collectible->transform.position, glm::vec3(1.0f, 0.8f, 0.2f), 20);
                        
                        // Check if all shards collected
                        if (shardsCollected >= totalShards && !mainGemActive) {
                            mainGemActive = true;
                            mainGem->isActive = true;
                            std::cout << "ALL COINS COLLECTED! Main Gem appeared!" << std::endl;
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
        std::cout << "Level 1 COMPLETE!" << std::endl;
    }
}

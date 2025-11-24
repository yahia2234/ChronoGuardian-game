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
    glm::vec3 rockColor(0.35f, 0.25f, 0.2f); // Brown cave rock
    
    // Irregular cavern walls
    float roomWidth = 18.0f;
    float roomHeight = 12.0f;
    float roomDepth = 22.0f;
    
    // North wall (irregular)
    createWall(glm::vec3(0.0f, roomHeight/2, roomDepth/2), 
               glm::vec3(roomWidth, roomHeight, 1.0f), rockColor);
    walls.back()->materialType = 3; // Rock
    
    // South wall
    createWall(glm::vec3(0.0f, roomHeight/2, -roomDepth/2), 
               glm::vec3(roomWidth, roomHeight, 1.0f), rockColor);
    walls.back()->materialType = 3; // Rock
    
    // East wall (irregular)
    createWall(glm::vec3(roomWidth/2, roomHeight/2, 0.0f), 
               glm::vec3(1.0f, roomHeight, roomDepth), rockColor);
    walls.back()->materialType = 3; // Rock
    
    // West wall
    createWall(glm::vec3(-roomWidth/2, roomHeight/2, 0.0f), 
               glm::vec3(1.0f, roomHeight, roomDepth), rockColor);
    walls.back()->materialType = 3; // Rock
    
    // Ceiling
    createWall(glm::vec3(0.0f, roomHeight, 0.0f), 
               glm::vec3(roomWidth, 1.0f, roomDepth), rockColor);
    walls.back()->materialType = 3; // Rock
    
    // Uneven dirt floor
    glm::vec3 dirtColor(0.3f, 0.25f, 0.15f);
    createFloor(glm::vec3(0.0f, 0.0f, 0.0f), 
                glm::vec3(roomWidth, 0.1f, roomDepth), dirtColor);
    walls.back()->materialType = 3; // Rock/Dirt
}

void Level2::createStalactites() {
    // Create stalactites hanging from ceiling
    std::vector<glm::vec3> stalactitePositions = {
        glm::vec3(-4.0f, 10.0f, -2.0f),
        glm::vec3(3.0f, 10.0f, 1.0f),
        glm::vec3(-2.0f, 10.0f, 4.0f),
        glm::vec3(5.0f, 10.0f, 6.0f),
        glm::vec3(-5.0f, 10.0f, 8.0f)
    };
    
    for (const auto& pos : stalactitePositions) {
        auto stalactite = std::make_unique<Stalactite>(pos);
        objects.push_back(std::move(stalactite));
    }
}

void Level2::createGeysers() {
    // Create geyser vents in the floor
    std::vector<glm::vec3> geyserPositions = {
        glm::vec3(-3.0f, 0.0f, 0.0f),
        glm::vec3(4.0f, 0.0f, 3.0f),
        glm::vec3(-1.0f, 0.0f, 6.0f)
    };
    
    for (const auto& pos : geyserPositions) {
        auto geyser = std::make_unique<Geyser>(pos);
        objects.push_back(std::move(geyser));
    }
}

void Level2::createCollectible() {
    // Glowing Gemstone - rough-cut red gem
    auto gemstone = std::make_unique<Collectible>(
        glm::vec3(0.0f, 2.0f, 7.0f),
        glm::vec3(0.9f, 0.2f, 0.2f) // Red
    );
    objects.push_back(std::move(gemstone));
}

void Level2::createPedestal() {
    // Ancient stone pedestal
    auto ped = std::make_unique<GameObject>(GameObjectType::PEDESTAL);
    ped->transform.position = glm::vec3(0.0f, 0.5f, 10.0f);
    ped->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    ped->mesh.reset(Mesh::createCylinder(0.5f, 1.0f, 16));
    ped->color = glm::vec3(0.4f, 0.4f, 0.35f); // Stone grey
    ped->updateBoundingBox();
    ped->isTrigger = true;
    
    pedestal = ped.get();
    objects.push_back(std::move(ped));
}

void Level2::setupLighting() {
    // Multiple warm orange torches on walls
    std::vector<glm::vec3> torchPositions = {
        glm::vec3(-7.0f, 5.0f, -8.0f),
        glm::vec3(7.0f, 5.0f, -8.0f),
        glm::vec3(-7.0f, 5.0f, 0.0f),
        glm::vec3(7.0f, 5.0f, 0.0f),
        glm::vec3(-7.0f, 5.0f, 8.0f),
        glm::vec3(7.0f, 5.0f, 8.0f)
    };
    
    for (const auto& pos : torchPositions) {
        Light torch;
        torch.position = pos;
        torch.color = glm::vec3(1.0f, 0.6f, 0.3f); // Warm orange
        torch.intensity = 15.0f;
        torch.flickerSpeed = 5.0f + (rand() % 100) / 100.0f;
        torch.flickerAmount = 0.3f;
        lights.push_back(torch);
    }
    
    ambientLight = glm::vec3(0.1f); // Dim ambient
}

void Level2::update(float deltaTime, Player* player, ParticleSystem* particles) {
    Level::update(deltaTime, player, particles);
    
    // Check stalactite triggers (proximity)
    for (auto& obj : objects) {
        if (obj->type == GameObjectType::STALACTITE && obj->isActive) {
            Stalactite* stal = dynamic_cast<Stalactite*>(obj.get());
            if (stal && !stal->isFalling) {
                float dist = glm::length(player->getPosition() - stal->transform.position);
                if (dist < 2.0f) {
                    stal->onTrigger();
                }
            }
        }
    }
    
    // Check geyser push
    for (auto& obj : objects) {
        if (obj->type == GameObjectType::GEYSER && obj->isActive) {
            Geyser* geyser = dynamic_cast<Geyser*>(obj.get());
            if (geyser && geyser->isErupting) {
                // Push player
                if (Physics::checkSphereAABBCollision(player->collisionSphere, geyser->boundingBox)) {
                    player->transform.position += geyser->pushForce * deltaTime;
                }
                
                // Emit steam particles
                // White/Grey color, moving up
                particles->emit(
                    geyser->transform.position + glm::vec3(0.0f, 0.5f, 0.0f), // Start slightly above vent
                    glm::vec3(0.0f, 5.0f, 0.0f), // Upward velocity
                    glm::vec4(0.8f, 0.8f, 0.9f, 0.5f), // White-ish steam
                    0.2f, // Size
                    1.0f, // Lifetime
                    5 // Count per frame
                );
            }
        }
    }
    
    // Check if gem was collected and player reached pedestal
    if (hasCollectible && pedestal) {
        if (Physics::checkSphereAABBCollision(player->collisionSphere, pedestal->boundingBox)) {
            if (!gemPlaced) {
                gemPlaced = true;
                
                // Add pulsing light to gemstone on pedestal
                Light gemLight;
                gemLight.position = pedestal->transform.position + glm::vec3(0.0f, 1.0f, 0.0f);
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

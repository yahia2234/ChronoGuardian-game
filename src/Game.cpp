#include "Game.h"
#include "AudioManager.h"
#include "Input.h"
#include "Level1.h"
#include "Level2.h"
#include <iostream>

// Initialize static instance pointer
Game *Game::instance = nullptr;

Game::Game()
    : window(nullptr), screenWidth(1280), screenHeight(720),
      startScreenVAO(0), startScreenVBO(0), startScreenTime(0.0f),
      gameOverTime(0.0f), winScreenTime(0.0f),
      gameState(GameState::START_SCREEN), currentLevelIndex(0), deltaTime(0.0f),
      lastFrame(0.0f), running(true) {
  instance = this;
}

Game::~Game() { cleanup(); }

bool Game::init() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  // Configure GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Create window
  window = glfwCreateWindow(screenWidth, screenHeight, "Chrono Guardian",
                            nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return false;
  }

  // OpenGL configuration
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

  // Initialize input system
  Input::getInstance().init(window);

  // Initialize audio system
  if (!AudioManager::getInstance().init()) {
    std::cerr << "Warning: Audio system failed to initialize" << std::endl;
  }

  // Load shaders
  mainShader =
      std::make_unique<Shader>("shaders/vertex.glsl", "shaders/fragment.glsl");
  particleShader = std::make_unique<Shader>("shaders/particle_vertex.glsl",
                                            "shaders/particle_fragment.glsl");
  startScreenShader = std::make_unique<Shader>("shaders/start_screen_vertex.glsl",
                                                "shaders/start_screen_fragment.glsl");
  gameOverShader = std::make_unique<Shader>("shaders/game_over_vertex.glsl",
                                             "shaders/game_over_fragment.glsl");
  winScreenShader = std::make_unique<Shader>("shaders/win_screen_vertex.glsl",
                                              "shaders/win_screen_fragment.glsl");

  // Create camera
  camera = std::make_unique<Camera>();

  // Create player
  player = std::make_unique<Player>();

  // Create particle system
  particles = std::make_unique<ParticleSystem>(2000);

  // Initialize start screen
  initStartScreen();
  
  // Load heart model for UI
  heartModel = std::make_unique<Model>("assets/models/aztec_stone_heart.glb");

  std::cout << "Chrono Guardian - Controls:" << std::endl;
  std::cout << "  WASD - Move" << std::endl;
  std::cout << "  Mouse - Look around" << std::endl;
  std::cout << "  T - Toggle camera view" << std::endl;
  std::cout << "  R - Restart level" << std::endl;
  std::cout << "  ESC - Quit" << std::endl;

  return true;
}

void Game::run() {
  while (!glfwWindowShouldClose(window)) {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput();
    update();
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Game::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  if (instance) {
    instance->screenWidth = width;
    instance->screenHeight = height;
    glViewport(0, 0, width, height);
  }
}

void Game::processInput() {
  Input &input = Input::getInstance();

  if (input.isKeyPressed(KEY_ESC)) {
    glfwSetWindowShouldClose(window, true);
  }

  // Handle start screen - any key starts the game
  if (gameState == GameState::START_SCREEN) {
    if (anyKeyPressed()) {
      loadLevel(0);  // Start the game
    }
    return;  // Don't process other input on start screen
  }

  // Handle game over screen - any key restarts after a short delay
  if (gameState == GameState::GAME_OVER) {
    gameOverTime += deltaTime;
    if (gameOverTime > 1.0f && anyKeyPressed()) { // 1 second delay before accepting input
      // Reset the entire game
      player->resetHealth();  // Reset hearts to full
      loadLevel(0);  // Restart from Level 1
    }
    return;  // Don't process other input on game over screen
  }

  // Handle win screen - any key goes back to start screen
  if (gameState == GameState::WIN) {
    if (anyKeyPressed()) {
      // Reset the entire game state
      currentLevel.reset();  // Clear the current level
      currentLevelIndex = 0;  // Reset to beginning
      gameState = GameState::START_SCREEN;
      startScreenTime = 0.0f;  // Reset start screen timer
    }
    return;  // Don't process other input on win screen
  }

  if (input.isKeyJustPressed(KEY_R)) {
    restartLevel();
  }

  // Camera toggle with T key or Right Mouse Button
  if (input.isKeyJustPressed(KEY_T) ||
      input.isMouseButtonJustPressed(MOUSE_BUTTON_RIGHT)) {
    camera->toggleMode();
  }

  // Camera rotation with Q and E keys
  if (input.isKeyPressed(KEY_Q)) {
    camera->processMouseMovement(-1.0f, 0.0f); // Rotate left
  }
  if (input.isKeyPressed(KEY_E)) {
    camera->processMouseMovement(1.0f, 0.0f); // Rotate right
  }

  // Mouse look
  glm::vec2 mouseDelta = input.getMouseDelta();
  if (glm::length(mouseDelta) > 0.01f) {
    camera->processMouseMovement(mouseDelta.x, mouseDelta.y);
  }

  // No need to manually warp pointer, GLFW handles it with GLFW_CURSOR_DISABLED
}

void Game::update() {
  // Update start screen timer
  if (gameState == GameState::START_SCREEN) {
    startScreenTime += deltaTime;
    Input::getInstance().update();
    return;
  }

  // Update game over timer
  if (gameState == GameState::GAME_OVER) {
    gameOverTime += deltaTime;
    Input::getInstance().update();
    return;
  }

  // Update win screen timer
  if (gameState == GameState::WIN) {
    winScreenTime += deltaTime;
    Input::getInstance().update();
    return;
  }

  if (gameState == GameState::LEVEL1 || gameState == GameState::LEVEL2) {
    // Get movement input
    glm::vec3 moveInput = getMovementInput();

    // Update player
    player->update(deltaTime, moveInput);

    // Update camera
    if (camera->mode == CameraMode::FIRST_PERSON) {
      camera->setFirstPersonMode(player->getPosition());
    } else {
      camera->setThirdPersonMode(player->getPosition());

      // Camera Collision Check (Spring Arm)
      if (currentLevel) {
        glm::vec3 targetPos =
            player->getPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
        currentLevel->checkCameraCollision(camera->position, targetPos);
      }
    }
    camera->updateCameraVectors();

    // Update player's camera yaw for model rotation
    player->setCameraYaw(camera->yaw);

    // Update level
    if (currentLevel) {
      currentLevel->update(deltaTime, player.get(), particles.get());

      // Check if should reset to Level 1 (e.g., 3 stalactite hits in Level 2)
      if (currentLevel->shouldResetToLevel1) {
        AudioManager::getInstance().playSound(SoundEffect::TILE_CRACK, 0.7f);
        loadLevel(0); // Reset to Level 1
        return;
      }

      // Check if level should restart (e.g., player on disappeared tile)
      // Now shows game over screen instead of immediate restart
      if (currentLevel->shouldRestart) {
        gameState = GameState::GAME_OVER;
        gameOverTime = 0.0f;  // Reset game over timer
        AudioManager::getInstance().playSound(SoundEffect::TILE_CRACK, 0.7f);
        return;
      }

      // Check level completion
      if (currentLevel->levelComplete) {
        nextLevel();
      }
    }

    // Update particles
    particles->update(deltaTime);

    // Check if player fell off - trigger game over
    if (player->getPosition().y < -5.0f) {
      gameState = GameState::GAME_OVER;
      gameOverTime = 0.0f;  // Reset game over timer
      AudioManager::getInstance().playSound(SoundEffect::TILE_CRACK, 0.7f);
    }
    
    // Check if player is dead (no hearts left)
    if (player->isDead()) {
      gameState = GameState::GAME_OVER;
      gameOverTime = 0.0f;
      AudioManager::getInstance().playSound(SoundEffect::TILE_CRACK, 0.7f);
    }
  }

  // Clear mouse delta AFTER processing
  Input::getInstance().clearMouseDelta();

  // Update input state for next frame
  Input::getInstance().update();
}

void Game::renderText(float x, float y, const std::string &text) {
  // Text rendering is temporarily disabled during GLFW migration
  // as GLUT bitmap fonts are not available.
  // TODO: Implement FreeType or similar font rendering.
}

void Game::render() {
  // Render start screen
  if (gameState == GameState::START_SCREEN) {
    renderStartScreen();
    return;
  }

  // Render game over screen
  if (gameState == GameState::GAME_OVER) {
    renderGameOver();
    return;
  }

  // Render win screen
  if (gameState == GameState::WIN) {
    renderWinScreen();
    return;
  }

  // Normal sky blue background
  glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (gameState == GameState::LEVEL1 || gameState == GameState::LEVEL2) {
    // Set up main shader
    mainShader->use();

    // Initialize texture sampler to use texture unit 0
    mainShader->setInt("textureSampler", 0);

    glm::mat4 projection =
        camera->getProjectionMatrix((float)screenWidth / screenHeight);
    glm::mat4 view = camera->getViewMatrix();

    mainShader->setMat4("projection", projection);
    mainShader->setMat4("view", view);
    mainShader->setVec3("viewPos", camera->position);
    mainShader->setFloat("time", glfwGetTime());
    mainShader->setFloat("transparency", 1.0f); // Ensure normal objects are fully opaque

    // Set lighting
    if (currentLevel) {
      currentLevel->drawLights(mainShader.get());
    }

    // Draw level
    if (currentLevel) {
      currentLevel->draw(mainShader.get());
      currentLevel->drawLightFixtureModels(mainShader.get());  // Draw the fractured orb models
    }

    // Draw player (only if in Third Person mode)
    if (camera->mode == CameraMode::THIRD_PERSON) {
      player->draw(mainShader.get());
    }

    // Draw particles
    particleShader->use();
    particleShader->setMat4("projection", projection);
    particleShader->setMat4("view", view);
    particles->draw(view, projection);

    // Draw damage flash overlay (CoD style - thin edges with blood splatter lines)
    if (player->damageFlashIntensity > 0.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST); // Draw on top of everything

        mainShader->use();
        glm::mat4 identity = glm::mat4(1.0f);
        mainShader->setMat4("projection", identity);
        mainShader->setMat4("view", identity);
        mainShader->setVec3("objectColor", glm::vec3(0.8f, 0.0f, 0.0f)); // Dark red
        mainShader->setBool("useTexture", false);
        mainShader->setFloat("emissive", 1.0f); // Make it glow (no lighting calc)
        mainShader->setVec3("viewPos", glm::vec3(0,0,1)); // Dummy
        mainShader->setInt("numLights", 0); // No lights for overlay
        
        static std::unique_ptr<Mesh> flashQuad = std::unique_ptr<Mesh>(Mesh::createCube(1.0f));
        
        float alpha = player->damageFlashIntensity * 0.35f; // More transparent (max 35%)
        mainShader->setFloat("transparency", alpha);
        
        float edgeWidth = 0.08f; // Thinner edges
        
        // Left edge (main)
        mainShader->setMat4("model", glm::translate(identity, glm::vec3(-1.0f + edgeWidth, 0.0f, 0.0f)) * 
                                     glm::scale(identity, glm::vec3(edgeWidth * 2.0f, 2.0f, 0.01f)));
        flashQuad->draw();
        
        // Right edge (main)
        mainShader->setMat4("model", glm::translate(identity, glm::vec3(1.0f - edgeWidth, 0.0f, 0.0f)) * 
                                     glm::scale(identity, glm::vec3(edgeWidth * 2.0f, 2.0f, 0.01f)));
        flashQuad->draw();
        
        // Blood splatter lines - diagonal streaks from corners
        float lineAlpha = alpha * 0.7f; // Slightly more transparent
        mainShader->setFloat("transparency", lineAlpha);
        
        // Top-left diagonal lines
        for (int i = 0; i < 3; i++) {
            float offset = i * 0.12f;
            glm::mat4 lineModel = glm::translate(identity, glm::vec3(-0.85f + offset, 0.75f - offset * 0.5f, 0.0f));
            lineModel = glm::rotate(lineModel, glm::radians(-45.0f), glm::vec3(0, 0, 1));
            lineModel = glm::scale(lineModel, glm::vec3(0.25f - i * 0.05f, 0.015f, 0.01f));
            mainShader->setMat4("model", lineModel);
            flashQuad->draw();
        }
        
        // Top-right diagonal lines  
        for (int i = 0; i < 3; i++) {
            float offset = i * 0.12f;
            glm::mat4 lineModel = glm::translate(identity, glm::vec3(0.85f - offset, 0.75f - offset * 0.5f, 0.0f));
            lineModel = glm::rotate(lineModel, glm::radians(45.0f), glm::vec3(0, 0, 1));
            lineModel = glm::scale(lineModel, glm::vec3(0.25f - i * 0.05f, 0.015f, 0.01f));
            mainShader->setMat4("model", lineModel);
            flashQuad->draw();
        }
        
        // Bottom-left diagonal lines
        for (int i = 0; i < 2; i++) {
            float offset = i * 0.15f;
            glm::mat4 lineModel = glm::translate(identity, glm::vec3(-0.8f + offset, -0.7f + offset * 0.4f, 0.0f));
            lineModel = glm::rotate(lineModel, glm::radians(35.0f), glm::vec3(0, 0, 1));
            lineModel = glm::scale(lineModel, glm::vec3(0.2f - i * 0.04f, 0.012f, 0.01f));
            mainShader->setMat4("model", lineModel);
            flashQuad->draw();
        }
        
        // Bottom-right diagonal lines
        for (int i = 0; i < 2; i++) {
            float offset = i * 0.15f;
            glm::mat4 lineModel = glm::translate(identity, glm::vec3(0.8f - offset, -0.7f + offset * 0.4f, 0.0f));
            lineModel = glm::rotate(lineModel, glm::radians(-35.0f), glm::vec3(0, 0, 1));
            lineModel = glm::scale(lineModel, glm::vec3(0.2f - i * 0.04f, 0.012f, 0.01f));
            mainShader->setMat4("model", lineModel);
            flashQuad->draw();
        }
        
        // Restore state
        glEnable(GL_DEPTH_TEST);
        mainShader->setFloat("emissive", 0.0f);
        mainShader->setFloat("transparency", 1.0f);
    }
    
    // Render hearts UI
    renderHearts();
  }
}

void Game::renderHearts() {
  mainShader->use();
  
  // Set up orthographic-like projection for UI rendering
  glm::mat4 identity = glm::mat4(1.0f);
  mainShader->setMat4("projection", identity);
  mainShader->setMat4("view", identity);
  mainShader->setBool("useTexture", false);
  mainShader->setFloat("transparency", 1.0f);
  mainShader->setVec3("viewPos", glm::vec3(0, 0, 1));
  mainShader->setInt("numLights", 0);
  
  glDisable(GL_DEPTH_TEST); // Draw on top
  glDisable(GL_CULL_FACE);  // Hearts are 2D, render both sides
  
  // Create a heart mesh for UI (static)
  static std::unique_ptr<Mesh> heartMesh = std::unique_ptr<Mesh>(Mesh::createHeart(1.0f));
  
  // Draw hearts in top-right corner - MUCH BIGGER
  float heartSize = 0.05f;   // MUCH bigger hearts
  float startX = 0.35f;      // Start position
  float startY = 0.80f;      // Position from top
  float spacing = 0.15f;     // Spacing between hearts
  
  // Draw max hearts (4) - grey for lost hearts, red for current hearts
  for (int i = 0; i < player->maxHearts; i++) {
    float xPos = startX + i * spacing;
    float yPos = startY;
    
    glm::mat4 model = glm::translate(identity, glm::vec3(xPos, yPos, 0.0f));
    model = glm::scale(model, glm::vec3(heartSize, heartSize, 1.0f));
    
    if (i < player->getHearts()) {
      // Full heart - bright red with strong glow
      mainShader->setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.1f));
      mainShader->setFloat("emissive", 1.0f);
    } else {
      // Empty heart - dark outline
      mainShader->setVec3("objectColor", glm::vec3(0.3f, 0.15f, 0.15f));
      mainShader->setFloat("emissive", 0.1f);
    }
    
    mainShader->setMat4("model", model);
    heartMesh->draw();
  }
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  mainShader->setFloat("emissive", 0.0f);
}

void Game::loadLevel(int levelIndex) {
  currentLevelIndex = levelIndex;

  if (levelIndex == 0) {
    currentLevel = std::make_unique<Level1>();
    gameState = GameState::LEVEL1;
    player->resetHealth(); // Reset hearts when starting Level 1
  } else if (levelIndex == 1) {
    currentLevel = std::make_unique<Level2>();
    gameState = GameState::LEVEL2;
    player->resetHealth(); // Reset hearts when entering Level 2
  } else {
    gameState = GameState::WIN;
    winScreenTime = 0.0f;  // Reset win screen timer
    return;
  }

  currentLevel->init();
  player->reset(currentLevel->playerStartPosition);  // Fully reset player state
  particles->clear();
}

void Game::restartLevel() {
  // Play restart sound
  AudioManager::getInstance().playSound(SoundEffect::TILE_CRACK, 0.7f);
  loadLevel(currentLevelIndex);
}

void Game::nextLevel() {
  // Play level complete sound
  AudioManager::getInstance().playSound(SoundEffect::LEVEL_COMPLETE, 1.0f);

  loadLevel(currentLevelIndex + 1);
}

glm::vec3 Game::getMovementInput() {
  Input &input = Input::getInstance();
  glm::vec3 moveInput(0.0f);

  // Get camera direction vectors (flattened to XZ plane)
  glm::vec3 forward = camera->front;
  forward.y = 0.0f;
  if (glm::length(forward) > 0.001f) {
    forward = glm::normalize(forward);
  }

  glm::vec3 right = camera->right;
  right.y = 0.0f;
  if (glm::length(right) > 0.001f) {
    right = glm::normalize(right);
  }

  if (input.isKeyPressed(KEY_W)) {
    moveInput += forward;
  }
  if (input.isKeyPressed(KEY_S)) {
    moveInput -= forward;
  }
  if (input.isKeyPressed(KEY_A)) {
    moveInput -= right;
  }
  if (input.isKeyPressed(KEY_D)) {
    moveInput += right;
  }

  // Normalize to prevent faster diagonal movement
  if (glm::length(moveInput) > 0.0f) {
    moveInput = glm::normalize(moveInput);
  }

  return moveInput;
}

void Game::initStartScreen() {
  // Create a fullscreen quad for the start screen
  float quadVertices[] = {
      // positions   // texCoords
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f, 0.0f,

      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f, 1.0f
  };

  glGenVertexArrays(1, &startScreenVAO);
  glGenBuffers(1, &startScreenVBO);
  
  glBindVertexArray(startScreenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, startScreenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  
  // Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  // TexCoord attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  
  glBindVertexArray(0);
}

void Game::renderStartScreen() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glDisable(GL_DEPTH_TEST);
  
  startScreenShader->use();
  startScreenShader->setFloat("time", startScreenTime);
  
  glBindVertexArray(startScreenVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  
  glEnable(GL_DEPTH_TEST);
}

void Game::renderGameOver() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glDisable(GL_DEPTH_TEST);
  
  gameOverShader->use();
  gameOverShader->setFloat("time", gameOverTime);
  
  // Reuse the same VAO as start screen (fullscreen quad)
  glBindVertexArray(startScreenVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  
  glEnable(GL_DEPTH_TEST);
}

void Game::renderWinScreen() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glDisable(GL_DEPTH_TEST);
  
  winScreenShader->use();
  winScreenShader->setFloat("time", winScreenTime);
  
  // Reuse the same VAO as start screen (fullscreen quad)
  glBindVertexArray(startScreenVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  
  glEnable(GL_DEPTH_TEST);
}

bool Game::anyKeyPressed() {
  // Check for any key press (except ESC which quits)
  for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
    if (key == GLFW_KEY_ESCAPE) continue;
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      return true;
    }
  }
  
  // Also check for mouse button press
  for (int button = 0; button < 3; button++) {
    if (glfwGetMouseButton(window, button) == GLFW_PRESS) {
      return true;
    }
  }
  
  return false;
}

void Game::cleanup() {
  // Clean up start screen resources
  if (startScreenVAO) {
    glDeleteVertexArrays(1, &startScreenVAO);
  }
  if (startScreenVBO) {
    glDeleteBuffers(1, &startScreenVBO);
  }
  
  AudioManager::getInstance().cleanup();
  glfwTerminate();
}

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
      gameState(GameState::LEVEL1), currentLevelIndex(0), deltaTime(0.0f),
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

  // Create camera
  camera = std::make_unique<Camera>();

  // Create player
  player = std::make_unique<Player>();

  // Create particle system
  particles = std::make_unique<ParticleSystem>(2000);

  // Load first level
  loadLevel(0);

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

  if (input.isKeyJustPressed(KEY_R)) {
    restartLevel();
  }

  // Restart from Win Screen
  if (gameState == GameState::WIN && input.isKeyJustPressed(KEY_R)) {
    loadLevel(0); // Restart from Level 1
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
      if (currentLevel->shouldRestart) {
        restartLevel();
        return;
      }

      // Check level completion
      if (currentLevel->levelComplete) {
        nextLevel();
      }
    }

    // Update particles
    particles->update(deltaTime);

    // Check if player fell off
    if (player->getPosition().y < -5.0f) {
      restartLevel();
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
  if (gameState == GameState::WIN) {
    // Black background for win screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render "YOU WON!" - disabled
    // renderText(...)

    return;
  } else {
    // Normal sky blue background
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

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

    // Set lighting
    if (currentLevel) {
      currentLevel->drawLights(mainShader.get());
    }

    // Draw level
    if (currentLevel) {
      currentLevel->draw(mainShader.get());
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
  }
}

void Game::loadLevel(int levelIndex) {
  currentLevelIndex = levelIndex;

  if (levelIndex == 0) {
    currentLevel = std::make_unique<Level1>();
    gameState = GameState::LEVEL1;
  } else if (levelIndex == 1) {
    currentLevel = std::make_unique<Level2>();
    gameState = GameState::LEVEL2;
  } else {
    gameState = GameState::WIN;
    return;
  }

  currentLevel->init();
  player->setPosition(currentLevel->playerStartPosition);
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

void Game::cleanup() {
  AudioManager::getInstance().cleanup();
  glfwTerminate();
}

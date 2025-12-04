#ifndef GAME_H
#define GAME_H

#include "Camera.h"
#include "Level.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

enum class GameState { MENU, LEVEL1, LEVEL2, WIN, GAME_OVER };

class Game {
public:
  Game();
  ~Game();

  bool init();
  void run();
  void renderText(float x, float y, const std::string &text);
  void cleanup();

  // GLFW Callbacks
  static void framebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);

  static Game *instance; // Singleton for GLFW callbacks

private:
  GLFWwindow *window;
  int screenWidth;
  int screenHeight;

  std::unique_ptr<Shader> mainShader;
  std::unique_ptr<Shader> particleShader;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Player> player;
  std::unique_ptr<Level> currentLevel;
  std::unique_ptr<ParticleSystem> particles;

  GameState gameState;
  int currentLevelIndex;

  float deltaTime;
  float lastFrame;
  bool running;

  void processInput();
  void update();
  void render();

  void loadLevel(int levelIndex);
  void restartLevel();
  void nextLevel();

  glm::vec3 getMovementInput();
};

#endif

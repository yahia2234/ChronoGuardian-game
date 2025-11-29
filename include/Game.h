#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "Camera.h"
#include "Level.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "Shader.h"
#include <memory>
#include <string>

enum class GameState { MENU, LEVEL1, LEVEL2, WIN, GAME_OVER };

class Game {
public:
  Game();
  ~Game();

  bool init(int argc, char **argv);
  void run();
  void renderText(float x, float y, const std::string &text,
                  void *font = GLUT_BITMAP_HELVETICA_18);
  void cleanup();

  // GLUT callbacks (must be static)
  static void displayCallback();
  static void reshapeCallback(int width, int height);
  static void timerCallback(int value);
  static void idleCallback();

  static Game *instance; // Singleton for GLUT callbacks

private:
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

#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Shader.h"
#include "Camera.h"
#include "Player.h"
#include "Level.h"
#include "ParticleSystem.h"

enum class GameState {
    MENU,
    LEVEL1,
    LEVEL2,
    WIN,
    GAME_OVER
};

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void cleanup();

private:
    GLFWwindow* window;
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

    void processInput();
    void update();
    void render();

    void loadLevel(int levelIndex);
    void restartLevel();
    void nextLevel();

    glm::vec3 getMovementInput();
};

#endif

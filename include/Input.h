#ifndef INPUT_H
#define INPUT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Define key constants using GLFW keys
#define KEY_W GLFW_KEY_W
#define KEY_A GLFW_KEY_A
#define KEY_S GLFW_KEY_S
#define KEY_D GLFW_KEY_D
#define KEY_Q GLFW_KEY_Q
#define KEY_E GLFW_KEY_E
#define KEY_R GLFW_KEY_R
#define KEY_T GLFW_KEY_T
#define KEY_SPACE GLFW_KEY_SPACE
#define KEY_ESC GLFW_KEY_ESCAPE

// Mouse buttons
// Mouse buttons
#define MOUSE_BUTTON_LEFT GLFW_MOUSE_BUTTON_LEFT
#define MOUSE_BUTTON_RIGHT GLFW_MOUSE_BUTTON_RIGHT
#define MOUSE_BUTTON_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE

class Input {
public:
  static Input &getInstance() {
    static Input instance;
    return instance;
  }

  void init(GLFWwindow *window);
  void update();
  void clearMouseDelta();

  bool isKeyPressed(int key) const;
  bool isKeyJustPressed(int key) const;
  bool isMouseButtonPressed(int button) const;
  bool isMouseButtonJustPressed(int button) const;

  glm::vec2 getMouseDelta() const { return mouseDelta; }
  glm::vec2 getMousePosition() const { return mousePosition; }

  // Manually set mouse position (useful when warping cursor)
  void setMousePosition(float x, float y);

  bool firstMouse;

  // GLUT Callbacks
  // GLFW Callbacks
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void mouseCallback(GLFWwindow *window, int button, int action,
                            int mods);
  static void cursorPositionCallback(GLFWwindow *window, double xpos,
                                     double ypos);

private:
  Input() : firstMouse(true), lastX(0), lastY(0) {}
  Input(const Input &) = delete;
  Input &operator=(const Input &) = delete;

  bool keys[1024];
  bool keysLastFrame[1024];
  bool mouseButtons[8];
  bool mouseButtonsLastFrame[8];

  glm::vec2 mousePosition;
  glm::vec2 mouseDelta;
  float lastX, lastY;
};

#endif

#ifndef INPUT_H
#define INPUT_H

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>

// Define key constants to match previous GLFW usage (mapped to ASCII or GLUT
// special)
#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'
#define KEY_Q 'q'
#define KEY_E 'e'
#define KEY_R 'r'
#define KEY_T 't'
#define KEY_SPACE 32
#define KEY_ESC 27

// Mouse buttons
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1
#define MOUSE_BUTTON_MIDDLE 2

class Input {
public:
  static Input &getInstance() {
    static Input instance;
    return instance;
  }

  void init();
  void update();
  void clearMouseDelta();

  bool isKeyPressed(int key) const;
  bool isKeyJustPressed(int key) const;
  bool isMouseButtonPressed(int button) const;
  bool isMouseButtonJustPressed(int button) const;

  glm::vec2 getMouseDelta() const { return mouseDelta; }
  glm::vec2 getMousePosition() const { return mousePosition; }

  bool firstMouse;

  // GLUT Callbacks
  static void keyboardCallback(unsigned char key, int x, int y);
  static void keyboardUpCallback(unsigned char key, int x, int y);
  static void specialCallback(int key, int x, int y);
  static void specialUpCallback(int key, int x, int y);
  static void mouseCallback(int button, int state, int x, int y);
  static void motionCallback(int x, int y);
  static void passiveMotionCallback(int x, int y);

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

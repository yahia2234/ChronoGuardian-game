#include "Input.h"
#include <cstring>

void Input::init(GLFWwindow *window) {
  std::memset(keys, 0, sizeof(keys));
  std::memset(keysLastFrame, 0, sizeof(keysLastFrame));
  std::memset(mouseButtons, 0, sizeof(mouseButtons));
  std::memset(mouseButtonsLastFrame, 0, sizeof(mouseButtonsLastFrame));

  // Register GLFW callbacks
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetMouseButtonCallback(window, mouseCallback);

  // Disable cursor for FPS camera
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::update() {
  std::memcpy(keysLastFrame, keys, sizeof(keys));
  std::memcpy(mouseButtonsLastFrame, mouseButtons, sizeof(mouseButtons));
}

void Input::clearMouseDelta() { mouseDelta = glm::vec2(0.0f); }

void Input::setMousePosition(float x, float y) {
  lastX = x;
  lastY = y;
  mousePosition = glm::vec2(x, y);
}

bool Input::isKeyPressed(int key) const {
  if (key >= 0 && key < 1024)
    return keys[key];
  return false;
}

bool Input::isKeyJustPressed(int key) const {
  if (key >= 0 && key < 1024)
    return keys[key] && !keysLastFrame[key];
  return false;
}

bool Input::isMouseButtonPressed(int button) const {
  if (button >= 0 && button < 8)
    return mouseButtons[button];
  return false;
}

bool Input::isMouseButtonJustPressed(int button) const {
  if (button >= 0 && button < 8)
    return mouseButtons[button] && !mouseButtonsLastFrame[button];
  return false;
}

void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  Input &input = getInstance();
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      input.keys[key] = true;
    else if (action == GLFW_RELEASE)
      input.keys[key] = false;
  }
}

void Input::mouseCallback(GLFWwindow *window, int button, int action,
                          int mods) {
  Input &input = getInstance();
  if (button >= 0 && button < 8) {
    if (action == GLFW_PRESS)
      input.mouseButtons[button] = true;
    else if (action == GLFW_RELEASE)
      input.mouseButtons[button] = false;
  }
}

void Input::cursorPositionCallback(GLFWwindow *window, double xpos,
                                   double ypos) {
  Input &input = getInstance();

  if (input.firstMouse) {
    input.lastX = xpos;
    input.lastY = ypos;
    input.firstMouse = false;
  }

  input.mouseDelta.x = xpos - input.lastX;
  input.mouseDelta.y = input.lastY - ypos; // Reversed for consistency

  input.lastX = xpos;
  input.lastY = ypos;
  input.mousePosition = glm::vec2(xpos, ypos);
}

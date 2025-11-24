#include "Input.h"
#include <cstring>

void Input::init(GLFWwindow* win) {
    window = win;
    std::memset(keys, 0, sizeof(keys));
    std::memset(keysLastFrame, 0, sizeof(keysLastFrame));
    std::memset(mouseButtons, 0, sizeof(mouseButtons));
    std::memset(mouseButtonsLastFrame, 0, sizeof(mouseButtonsLastFrame));

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::update() {
    std::memcpy(keysLastFrame, keys, sizeof(keys));
    std::memcpy(mouseButtonsLastFrame, mouseButtons, sizeof(mouseButtons));
    // Do NOT clear mouseDelta here - it needs to be read by the game loop first!
}

void Input::clearMouseDelta() {
    mouseDelta = glm::vec2(0.0f);
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

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            input->keys[key] = true;
        else if (action == GLFW_RELEASE)
            input->keys[key] = false;
    }
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (button >= 0 && button < 8) {
        if (action == GLFW_PRESS)
            input->mouseButtons[button] = true;
        else if (action == GLFW_RELEASE)
            input->mouseButtons[button] = false;
    }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    
    if (input->firstMouse) {
        input->lastX = xpos;
        input->lastY = ypos;
        input->firstMouse = false;
    }

    input->mouseDelta.x = xpos - input->lastX;
    input->mouseDelta.y = input->lastY - ypos; // Reversed since y-coordinates go from bottom to top

    input->lastX = xpos;
    input->lastY = ypos;
    input->mousePosition = glm::vec2(xpos, ypos);
}

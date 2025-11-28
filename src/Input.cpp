#include "Input.h"
#include <cstring>

void Input::init() {
    std::memset(keys, 0, sizeof(keys));
    std::memset(keysLastFrame, 0, sizeof(keysLastFrame));
    std::memset(mouseButtons, 0, sizeof(mouseButtons));
    std::memset(mouseButtonsLastFrame, 0, sizeof(mouseButtonsLastFrame));

    // Register GLUT callbacks
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialCallback);
    glutSpecialUpFunc(specialUpCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(passiveMotionCallback);
    
    // Hide cursor and warp to center
    glutSetCursor(GLUT_CURSOR_NONE);
}

void Input::update() {
    std::memcpy(keysLastFrame, keys, sizeof(keys));
    std::memcpy(mouseButtonsLastFrame, mouseButtons, sizeof(mouseButtons));
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

void Input::keyboardCallback(unsigned char key, int x, int y) {
    Input& input = getInstance();
    if (key >= 0 && key < 1024) {
        input.keys[key] = true;
    }
}

void Input::keyboardUpCallback(unsigned char key, int x, int y) {
    Input& input = getInstance();
    if (key >= 0 && key < 1024) {
        input.keys[key] = false;
    }
}

void Input::specialCallback(int key, int x, int y) {
    Input& input = getInstance();
    // Map GLUT special keys to our key array (offset by 256 to avoid conflicts)
    int mappedKey = 256 + key;
    if (mappedKey < 1024) {
        input.keys[mappedKey] = true;
    }
}

void Input::specialUpCallback(int key, int x, int y) {
    Input& input = getInstance();
    int mappedKey = 256 + key;
    if (mappedKey < 1024) {
        input.keys[mappedKey] = false;
    }
}

void Input::mouseCallback(int button, int state, int x, int y) {
    Input& input = getInstance();
    
    // Map GLUT mouse buttons to our button array
    int mappedButton = -1;
    if (button == GLUT_LEFT_BUTTON) mappedButton = MOUSE_BUTTON_LEFT;
    else if (button == GLUT_RIGHT_BUTTON) mappedButton = MOUSE_BUTTON_RIGHT;
    else if (button == GLUT_MIDDLE_BUTTON) mappedButton = MOUSE_BUTTON_MIDDLE;
    
    if (mappedButton >= 0 && mappedButton < 8) {
        input.mouseButtons[mappedButton] = (state == GLUT_DOWN);
    }
}

void Input::motionCallback(int x, int y) {
    Input& input = getInstance();
    
    if (input.firstMouse) {
        input.lastX = x;
        input.lastY = y;
        input.firstMouse = false;
    }

    input.mouseDelta.x = x - input.lastX;
    input.mouseDelta.y = input.lastY - y; // Reversed for consistency

    input.lastX = x;
    input.lastY = y;
    input.mousePosition = glm::vec2(x, y);
}

void Input::passiveMotionCallback(int x, int y) {
    // Same as motion callback but for when no button is pressed
    motionCallback(x, y);
}

#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input {
public:
    static Input& getInstance() {
        static Input instance;
        return instance;
    }

    void init(GLFWwindow* window);
    void update();
    void clearMouseDelta();
 // New method to clear delta at end of frame

    bool isKeyPressed(int key) const;

    bool isKeyJustPressed(int key) const;
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonJustPressed(int button) const;

    glm::vec2 getMouseDelta() const { return mouseDelta; }
    glm::vec2 getMousePosition() const { return mousePosition; }

    bool firstMouse;

private:
    Input() : firstMouse(true), lastX(0), lastY(0) {}
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    GLFWwindow* window;
    bool keys[1024];
    bool keysLastFrame[1024];
    bool mouseButtons[8];
    bool mouseButtonsLastFrame[8];
    
    glm::vec2 mousePosition;
    glm::vec2 mouseDelta;
    float lastX, lastY;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif

#pragma once
#include "shared.h"

struct Engine
{
public:
    Engine::Engine(GLFWwindow* window_);
    void onKeyPress(int key, int scancode, int action, int mods);
    Camera getCamera();

private:
    GLFWwindow *window;
    Camera camera;

    void moveCamera(glm::vec3 direction);
    void tiltCamera(glm::vec3 direction);
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (engine)
        engine->onKeyPress(key, scancode, action, mods);
}

Engine::Engine(GLFWwindow *window_)
{
    window = window_;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

void Engine::onKeyPress(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.5, 0.0, 0.0));
    }
    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(-0.5, 0.0, 0.0));
    }
    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, 0.0, -0.5));
    }
    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, 0.0, 0.5));
    }
    if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, 0.5, 0.0));
    }
    if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, -0.5, 0.0));
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        tiltCamera(glm::vec3(-0.5, 0.0, 0.0));
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        tiltCamera(glm::vec3(0.5, 0.0, 0.0));
    }
    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        tiltCamera(glm::vec3(0.0, 0.5, 0.0));
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        tiltCamera(glm::vec3(0.0, -0.5, 0.0));
    }
}

void Engine::moveCamera(glm::vec3 direction) {
    camera.cameraPos += direction;
    camera.targetPos += direction;
}

void Engine::tiltCamera(glm::vec3 direction)
{
    camera.targetPos += direction;
}

Camera Engine::getCamera() {
    return camera;
}

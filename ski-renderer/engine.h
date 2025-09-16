#pragma once
#include "shared.h"

struct Engine
{
public:
    Engine::Engine(GLFWwindow *window_);
    void onKeyPress(int key, int scancode, int action, int mods);
    const Camera& getCamera() const;

private:
    GLFWwindow *window;
    Camera camera;
    bool first = true;

    void moveCamera(glm::vec3 direction);
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
    else if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(-0.5, 0.0, 0.0));
    }
    else if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, 0.0, -0.5));
    }
    else if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, 0.0, 0.5));
    }
    else if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, 0.5, 0.0));
    }
    else if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        moveCamera(glm::vec3(0.0, -0.5, 0.0));
    }
}

void Engine::moveCamera(glm::vec3 direction)
{
    camera.cameraPos += direction;
    camera.targetPos += direction;
}

const Camera& Engine::getCamera() const
{
    return camera;
}

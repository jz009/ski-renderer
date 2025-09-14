#pragma once
#include "shared.h"

struct Engine
{
public:
    Engine::Engine(GLFWwindow* window_);
    void onKeyPress(int key, int scancode, int action, int mods);

private:
    GLFWwindow *window;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    printf("reww");
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (engine)
        engine->onKeyPress(key, scancode, action, mods);
}

Engine::Engine(GLFWwindow *window_)
{
    printf("blah");
    window = window_;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

void Engine::onKeyPress(int key, int scancode, int action, int mods)
{
    int blah = mods + scancode;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        printf("%d", blah);
    }
}

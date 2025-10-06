#pragma once
#include "includes_fwd.h"

#include "input.h"

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input)
        input->onKeyPress(key, scancode, action, mods);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input)
        input->onMouseClick(button, action, mods);
}

static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input)
        input->onMouseMove({ (float)xpos, (float)ypos });
}

void Input::clear()
{
    keyboardInput.fresh = false;
    mouseClickInput.fresh = false;
}

Input::Input(GLFWwindow* window_)
{
    window = window_;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);
}

void Input::onKeyPress(int key, int scancode, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    keyboardInput = { true, key, scancode, action, mods, glm::vec2((float)xpos, (float)ypos) };
}

void Input::onMouseClick(int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    mouseClickInput = { true, button, action, mods, glm::vec2((float)xpos, (float)ypos) };
}

void Input::onMouseMove(glm::vec2 mousePos)
{
    mousePosition = mousePos;
}
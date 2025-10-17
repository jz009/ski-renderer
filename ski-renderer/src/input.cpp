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
    keyState = {};
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::onKeyPress(int key, int scancode, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    keyboardInput = { true, key, scancode, action, mods, glm::vec2((float)xpos, (float)ypos) };
    if (action == GLFW_PRESS)
    {
        keyState[key] = 1;
    }
    else if (action == GLFW_RELEASE)
    {
        keyState[key] = 0;
    }

    if (keyState[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
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

void Input::setMouseModeVisible() const {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::setMouseModeInvisible() const {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Input::isKeyDown(int key) const {
    return keyState[key];
}

bool Input::wasKeyPressed(int key) const {
    return keyboardInput.fresh && keyboardInput.key == key && keyboardInput.action == GLFW_PRESS;
}

bool Input::wasKeyReleased(int key) const {
    return keyboardInput.fresh && keyboardInput.key == key && keyboardInput.action == GLFW_RELEASE;
}

bool Input::wasMousePressed(int button) const {
    return mouseClickInput.fresh && mouseClickInput.button == button && mouseClickInput.action == GLFW_PRESS;
}
bool Input::wasMouseReleased(int button) const {
    return mouseClickInput.fresh && mouseClickInput.button == button && mouseClickInput.action == GLFW_RELEASE;
}
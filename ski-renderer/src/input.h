#pragma once
#include "includes_fwd.h"

struct KeyboardInput
{
    KeyboardInput::KeyboardInput() = default;
    bool fresh;
    int key;
    int scancode;
    int action;
    int mods;
    glm::vec2 mousePos;
};

struct MouseClickInput
{
    MouseClickInput::MouseClickInput() = default;
    bool fresh;
    int button;
    int action;
    int mods;
    glm::vec2 mousePos;
};

struct Input
{
    Input::Input(GLFWwindow* window_);
    void onKeyPress(int key, int scancode, int action, int mods);
    void onMouseClick(int button, int action, int mods);
    void onMouseMove(glm::vec2 mousePos);
    void clear();
    KeyboardInput keyboardInput = { false };
    MouseClickInput mouseClickInput = { false };
    glm::vec2 mousePosition;
    GLFWwindow* window;

    //Input() {}
};


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

struct MouseInput
{
    MouseInput::MouseInput() = default;
    bool fresh;
    int button;
    int action;
    int mods;
    glm::vec2 mousePos;
};

struct Input
{
public:
    Input::Input(GLFWwindow *window_);
    void init(GLFWwindow *window_);
    void onKeyPress(int key, int scancode, int action, int mods);
    void onMouseClick(int button, int action, int mods);
    void clear();
    KeyboardInput keyboardInput = {false};
    MouseInput mouseInput = {false};

private:
    GLFWwindow *window;
};

Input *getInput();

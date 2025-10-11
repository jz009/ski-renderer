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
    void setMouseModeVisible() const;
    void setMouseModeInvisible() const;
    bool isPressed(int key) const;
    bool wasPressed(int key) const;
    bool wasReleased(int key) const;
    bool wasMousePressed(int button) const;
    bool wasMouseReleased(int button) const;
    void clear();
    KeyboardInput keyboardInput = { false };
    MouseClickInput mouseClickInput = { false };
    std::array<bool, 512> keyState;
    glm::vec2 mousePosition;
    GLFWwindow* window;
};


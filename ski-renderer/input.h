#pragma once

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

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
    if (input)
        input->onKeyPress(key, scancode, action, mods);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
    if (input)
        input->onMouseClick(button, action, mods);
}

void Input::clear() {
    keyboardInput.fresh = false;
    mouseInput.fresh = false;
}

Input::Input(GLFWwindow *window_)
{
    window = window_;
    if (!window)
    {
        return;
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

void Input::onKeyPress(int key, int scancode, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    keyboardInput = {true, key, scancode, action, mods, glm::vec2((float)xpos, (float)ypos)};
}

void Input::onMouseClick(int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    mouseInput = {true, button, action, mods, glm::vec2((float)xpos, (float)ypos)};
}

void Input::init(GLFWwindow *window_)
{
    window = window_;
    if (!window)
    {
        return;
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

Input* getInput() {
    static Input INPUT(nullptr);
    return &INPUT;
}
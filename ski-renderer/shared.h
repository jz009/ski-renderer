#pragma once
#include "renderer.h"
#include "input.h"
#include <queue>

const float D2R = (float)M_PI / 180.f;
uint32_t WIDTH = 1500;
uint32_t HEIGHT = 1000;

std::filesystem::path mCUBE = "models/cube.obj";
std::filesystem::path sDEFAULT = "shaders/shader.wgsl";

struct Uniforms
{
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];
};

struct VertexAttributes
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Entity
{
    virtual void onFrame() {}
    virtual void onLoad() {}
};

struct Material
{
    wgpu::ShaderModule shader;
    Uniforms uniforms;
};

struct Model
{
    wgpu::Buffer buffer;
    uint32_t vertexCount;
    Material material;
    glm::vec3 scale = {1.0, 1.0, 1.0};
    glm::vec3 rotation;
    glm::vec3 position;
    glm::vec3 offset = {0.0, 0.0, 0.0};
};

struct Moveable
{
    glm::vec3 speed = {0.05, 0.05, 0.05};
    glm::vec3 targetPosition;
};

std::queue<Model> models;
void updateModel(Model &model);
void move(const Moveable &moveable, glm::vec3 &position);

struct Terrain : Entity
{
    Model model;
    void onFrame() override {
        updateModel(model);
        models.push(model);
    }
    void onLoad() override {}
};

glm::vec3 getMouseWorld(
    float mouseX, float mouseY,
    const glm::mat4& view,
    const glm::mat4& proj
) {
    glm::vec4 viewport(0, 0, WIDTH, HEIGHT);

    glm::vec3 nearPoint = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 0.0f), view, proj, viewport);
    glm::vec3 farPoint  = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 1.0f), view, proj, viewport);
    
    glm::vec3 dir = farPoint - nearPoint;

    float t = -nearPoint.y / dir.y;
    return nearPoint + t * dir;
}

struct Player : Entity
{
    Model model;
    Moveable moveable;
    void onFrame() override
    {
        if (getInput()->keyboardInput.fresh)
        {
            glm::vec2 mousePos = glm::vec2(getInput()->keyboardInput.mousePos);
            moveable.targetPosition = getMouseWorld(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
        }

        move(moveable, model.position);
        updateModel(model);
        models.push(model);
    }
    void onLoad() override {}
};

struct Camera
{
    glm::vec3 cameraPos;
    glm::vec3 targetPos;

    Camera::Camera()
    {
        cameraPos = glm::vec3(3.0f, 15.0f, 10.0f);
        targetPos = glm::vec3(0.0f, 3.0f, 0.0f);
    }
    Camera(const Camera &) = delete;
    Camera operator=(const Camera &) const = delete;
};

void move(const Moveable &moveable, glm::vec3 &position)
{
    position = glm::mix(position, moveable.targetPosition, 0.05f);
    // if (glm::length(position - moveable.targetPosition) > 1.0)
    // {
    //     glm::vec3 direction = glm::normalize(position - moveable.targetPosition);
    //     step = direction * moveable.speed;
    //     position += step;
    // }
}

void updateModel(Model &model)
{
    Uniforms uniforms = model.material.uniforms;
    model.material.uniforms.modelMatrix = glm::scale(glm::mat4x4(1.0), model.scale) * glm::translate(glm::mat4x4(1.0), model.position) * glm::translate(glm::mat4x4(1.0), model.offset);
}

Uniforms getDefaultUniforms()
{
    Uniforms uniforms;
    uniforms.time = static_cast<float>(glfwGetTime());
    uniforms.color = {0.0f, 1.0f, 0.4f, 1.0f};

    glm::vec3 focalPoint(0.0, 0.0, -2.0);
    float ratio = (float)WIDTH / (float)HEIGHT;
    float focalLength = 1.0;
    float near = 0.01f;
    float far = 100.0f;
    uniforms.modelMatrix = glm::mat4x4(1.0);

    glm::vec3 cameraPosition = glm::vec3(4.0f, 10.0f, 10.0f);
    glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    uniforms.viewMatrix = glm::lookAt(cameraPosition, targetPosition, upVector);

    float fov = 2 * glm::atan(1 / focalLength);
    uniforms.projectionMatrix = glm::perspective(fov, ratio, near, far);
    return uniforms;
}

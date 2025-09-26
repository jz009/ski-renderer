#pragma once
#include "renderer.h"
#include "input.h"
#include <queue>

const float D2R = (float)M_PI / 180.f;
uint32_t WIDTH = 1500;
uint32_t HEIGHT = 1000;

std::filesystem::path mCUBE = "models/cube.obj";
std::filesystem::path sDEFAULT = "shaders/shader.wgsl";

void printVec(glm::vec3 vec)
{
    printf("%f, %f, %f\n", vec.x, vec.y, vec.z);
}

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

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

void printAABB(AABB aabb)
{
    printf("%f, %f, %f, %f, %f, %f\n", aabb.min.x, aabb.max.x, aabb.min.y, aabb.max.y, aabb.min.z, aabb.max.z);
}

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
    AABB box;
    AABB adjustedBox;
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

void adjustAABB(Model &model)
{
    model.adjustedBox.min = glm::vec3(glm::vec4(model.box.min, 0) * glm::scale(glm::mat4x4(1.0), model.scale) * glm::translate(glm::mat4x4(1.0), model.position) * glm::translate(glm::mat4x4(1.0), model.offset));
    model.adjustedBox.max = glm::vec3(glm::vec4(model.box.max, 0) * glm::scale(glm::mat4x4(1.0), model.scale) * glm::translate(glm::mat4x4(1.0), model.position) * glm::translate(glm::mat4x4(1.0), model.offset));
}

std::deque<Model> models;
void updateModel(Model &model);
void move(const Moveable &moveable, glm::vec3 &position);

struct Terrain : Entity
{
    Model model;
    void onFrame() override
    {
        updateModel(model);
        models.push_front(model);
    }
    void onLoad() override {}
};

std::deque<Model> getModels()
{
    return models;
}

glm::vec3 getMouseWorld(
    float mouseX, float mouseY,
    const glm::mat4 &view,
    const glm::mat4 &proj)
{
    glm::vec4 viewport(0, 0, WIDTH, HEIGHT);

    glm::vec3 nearPoint = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 0.0f), view, proj, viewport);
    glm::vec3 farPoint = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 1.0f), view, proj, viewport);

    glm::vec3 dir = farPoint - nearPoint;

    float t = -nearPoint.y / dir.y;
    return nearPoint + t * dir;
}

bool pointInAABB(glm::vec3 point, AABB box)
{
    return (
        point.x >= box.min.x &&
        point.x <= box.max.x &&
        point.y >= box.min.y &&
        point.y <= box.max.y&&
        point.z >= box.min.z &&
        point.z <= box.max.z);
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
            auto clickLocation = getMouseWorld(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
            for (Model m : getModels())
            {
                if (pointInAABB(clickLocation - glm::vec3(0.0, -0.1, 0.0), m.adjustedBox))
                {
                    moveable.targetPosition = clickLocation;
                    break;
                }
            }
        }

        move(moveable, model.position);
        updateModel(model);
        models.push_front(model);
    }
    void onLoad() override {}
};

struct Camera
{
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float ratio;
    float focalLength;
    float near;
    float far;

    Camera::Camera()
    {
        position = glm::vec3(4.0f, 10.0f, 10.0f);
        target = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        ratio = (float)WIDTH / (float)HEIGHT;
        focalLength = 1.0;
        near = 0.01f;
        far = 100.0f;
    }
    Camera(const Camera &) = delete;
    Camera operator=(const Camera &) const = delete;
};

void move(const Moveable &moveable, glm::vec3 &position)
{
    position = glm::mix(position, moveable.targetPosition, 0.05f);
}

void updateModel(Model &model)
{
    Uniforms uniforms = model.material.uniforms;
    adjustAABB(model);
    model.material.uniforms.modelMatrix = glm::scale(glm::mat4x4(1.0), model.scale) * glm::translate(glm::mat4x4(1.0), model.position) * glm::translate(glm::mat4x4(1.0), model.offset);
}

Uniforms getDefaultUniforms()
{
    Camera camera;
    Uniforms uniforms;

    float fov = 2 * glm::atan(1 / camera.focalLength);

    uniforms.time = static_cast<float>(glfwGetTime());
    uniforms.color = {0.0f, 1.0f, 0.4f, 1.0f};
    uniforms.modelMatrix = glm::mat4x4(1.0);
    uniforms.viewMatrix = glm::lookAt(camera.position, camera.target, camera.up);
    uniforms.projectionMatrix = glm::perspective(fov, camera.ratio, camera.near, camera.far);

    return uniforms;
}

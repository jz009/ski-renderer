#pragma once

#include "input.h"

struct Constants
{
    static constexpr float D2R = (float)M_PI / 180.f;
    static constexpr uint32_t WIDTH = 1500;
    static constexpr uint32_t HEIGHT = 1000;

    static const std::string mCUBE; 
    static const std::string sDEFAULT; 
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

    Camera::Camera();

    Camera(const Camera &) = delete;
    Camera operator=(const Camera &) const = delete;
};

struct Uniforms
{
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];

    Uniforms()
    {
        Camera camera;
        float fov = 2 * glm::atan(1 / camera.focalLength);
        time = static_cast<float>(glfwGetTime());
        color = {0.0f, 1.0f, 0.4f, 1.0f};
        modelMatrix = glm::mat4x4(1.0);
        viewMatrix = glm::lookAt(camera.position, camera.target, camera.up);
        projectionMatrix = glm::perspective(fov, camera.ratio, camera.near, camera.far);
    }
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

struct VertexAttributes
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct ObjResult
{
    std::vector<VertexAttributes> vertexData;
    AABB box;
};

struct Material
{
    wgpu::ShaderModule shader;
    Uniforms uniforms;
};

struct Transform
{
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 position;
    glm::vec3 offset;

    Transform::Transform() = default;
    Transform(glm::vec3 _scale, glm::vec3 _rotation, glm::vec3 _position, glm::vec3 _offset)
    {
        scale = _scale;
        rotation = _rotation;
        position = _position;
        offset = _offset;
    }
};

struct Model
{
    wgpu::Buffer buffer;
    uint32_t vertexCount;
    Material material;
};

struct Movement
{
    glm::vec3 speed = {0.05, 0.05, 0.05};
    glm::vec3 targetPosition;
};

glm::vec3 move(const Movement &movement, glm::vec3 position);
void updateTransform(Model &model, const Transform &transform);
ObjResult loadObj(const std::filesystem::path &geometry);
glm::vec3 getMouseWorld(float mouseX, float mouseY, const glm::mat4 &view, const glm::mat4 &proj);
std::vector<VertexAttributes> load2D(const std::filesystem::path &geometry);


#pragma once
#include "renderer.h"

const float D2R = (float)M_PI / 180.f;
uint32_t WIDTH = 1000;
uint32_t HEIGHT = 1500;

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
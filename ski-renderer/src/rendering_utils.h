#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "AABB.h"
#include "transform.h"

struct Uniforms
{
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];

    Uniforms::Uniforms() = default;
    Uniforms(Camera &camera)
    {
        float fov = 2 * glm::atan(1 / camera.focalLength);
        time = static_cast<float>(glfwGetTime());
        color = {0.0f, 1.0f, 0.4f, 1.0f};
        modelMatrix = glm::mat4x4(1.0);
        viewMatrix = glm::lookAt(camera.position, camera.target, camera.up);
        projectionMatrix = glm::perspective(fov, camera.ratio, camera.near, camera.far);
    }
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

struct Model
{
    wgpu::Buffer buffer;
    uint32_t vertexCount;
    Material material;
};

ObjResult loadObj(const std::filesystem::path& geometry);
std::vector<VertexAttributes> load2D(const std::filesystem::path& geometry);
void updateModel(Model& model, const Transform& transform, std::shared_ptr<Camera> camera);

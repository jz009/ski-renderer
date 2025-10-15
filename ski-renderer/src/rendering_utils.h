#pragma once
#include "includes_fwd.h"

#include "AABB.h"

struct Uniforms
{
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];

    Uniforms::Uniforms() = default;
    Uniforms(const Camera& camera);
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
glm::mat4x4 calculateModelMatrix(const Transform& transform);
glm::mat4x4 calculateViewMatrix(const Camera& camera);
glm::mat4x4 calculateProjectionMatrix(const Camera& camera);

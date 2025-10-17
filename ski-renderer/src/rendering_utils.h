#pragma once
#include "includes_fwd.h"

#include "AABB.h"

struct VisualUniforms
{
    glm::vec4 color;
};

struct TransformUniforms
{
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    TransformUniforms(const Camera& camera);
};

struct Pipeline {
    wgpu::RenderPipeline pipeline;
    wgpu::BindGroupLayout bindGroupLayout;
};

struct VertexAttributes
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
};

struct ObjResult
{
    std::vector<VertexAttributes> vertexData;
    AABB box;
};

struct PixelData {
    std::vector<uint8_t> pixels;
    unsigned int width;
    unsigned int height;
};

struct Material
{
    Pipeline pipeline;
    VisualUniforms uniforms;
    wgpu::TextureView texture;
    wgpu::Sampler sampler;
};

struct Model
{
    wgpu::Buffer buffer;
    uint32_t vertexCount;
    TransformUniforms transforms;
    Material material;

    Model::Model(const Camera& camera) : transforms(camera) {}
    Model::Model(const wgpu::Buffer& buffer, uint32_t vertexCount, const TransformUniforms& transforms, const Material& material) : buffer(buffer), vertexCount(vertexCount), transforms(transforms), material(material) {}
};

ObjResult loadObj(const std::filesystem::path& geometry);
std::vector<VertexAttributes> load2D(const std::filesystem::path& geometry);
glm::mat4x4 calculateModelMatrix(const Transform& transform);
glm::mat4x4 calculateViewMatrix(const Camera& camera);
glm::mat4x4 calculateProjectionMatrix(const Camera& camera);
PixelData loadBMP(const std::filesystem::path& filename);

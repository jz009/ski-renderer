#pragma once
#include "includes_fwd.h"

#include "rendering_utils.h"

struct Renderer
{
    struct PipelineDefaults
    {
        wgpu::PipelineLayout defaultLayout;
        wgpu::TextureDescriptor depthTextureDesc;
        wgpu::Texture depthTexture;
        wgpu::TextureViewDescriptor depthTextureViewDesc;
        wgpu::TextureView depthTextureView;
        wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth24Plus;
        wgpu::DepthStencilState defaultDepthStencilState;
        wgpu::BindGroupLayoutDescriptor defaultBindGroupLayoutDesc;
        wgpu::BindGroupLayoutEntry defaultBindingLayout;
        wgpu::BindGroupLayout defaultBindGroupLayout;
        wgpu::ColorTargetState defaultColorTarget;
        wgpu::BlendState defaultBlendState;
        wgpu::PipelineLayoutDescriptor defaultPipelineLayoutDescriptor;
    };

GLFWwindow *window;

Renderer::Renderer();
Renderer(const Renderer &) = delete;
Renderer operator=(const Renderer &) const = delete;

void terminate();
bool isRunning();

void draw(const Model& model);
void endFrame();
void beginFrame();

Model createModel(std::vector<VertexAttributes> vertexData, const Material& material);
Material createMaterial(const std::filesystem::path &shaderPath, const Uniforms& uniforms);

wgpu::TextureView getNextSurfaceTextureView();
void createRenderPass();

void initializePipelineDefaults();
void writeUniformBuffer(const Material& material);
void initializeBindGroups();
void createPipeline(wgpu::ShaderModule shaderModule);

ObjResult loadObj(const std::filesystem::path& geometry);
std::vector<VertexAttributes> load2D(const std::filesystem::path& geometry);

wgpu::Adapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);
wgpu::Device requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);

wgpu::Device device;
wgpu::Queue queue;
wgpu::Surface surface;
wgpu::TextureFormat surfaceFormat = wgpu::TextureFormat::Undefined;
wgpu::RenderPipeline pipeline;
wgpu::Buffer pointBuffer;
wgpu::Buffer indexBuffer;
wgpu::Buffer uniformBuffer;
wgpu::TextureView targetView;

wgpu::BindGroup bindGroup;

wgpu::RenderPassEncoder renderPass;
wgpu::CommandEncoder encoder;

PipelineDefaults pipelineDefaults;
};
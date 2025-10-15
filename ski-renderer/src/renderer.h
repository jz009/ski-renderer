#pragma once
#include "includes_fwd.h"

struct Renderer
{
    DISALLOW_IMPLICIT_COPIES(Renderer)
    
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

Model createModel(const std::vector<VertexAttributes>& vertexData, const Material& material);
Material createMaterial(const std::filesystem::path &shaderPath, const Uniforms& uniforms);
void terminate();
bool isRunning();
void draw(const Model& model);
wgpu::Buffer writeUniformBuffer(const Material& material);
wgpu::BindGroup  initializeBindGroups(const wgpu::Buffer& uniformBuffer);
wgpu::RenderPipeline createPipeline(const wgpu::ShaderModule& shaderModule);
void endFrame();
void beginFrame();
wgpu::TextureView getNextSurfaceTextureView();
void createRenderPass();
void initializePipelineDefaults();
void createTexture();

wgpu::Adapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);
wgpu::Device requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);

wgpu::Device device;
wgpu::Queue queue;
wgpu::Surface surface;
wgpu::TextureFormat surfaceFormat = wgpu::TextureFormat::Undefined;
wgpu::TextureView targetView;
wgpu::RenderPassEncoder renderPass;
wgpu::CommandEncoder encoder;
PipelineDefaults pipelineDefaults;
};
#pragma once
#include "includes_fwd.h"

#include "rendering_utils.h"

struct Renderer
{
    DISALLOW_IMPLICIT_COPIES(Renderer)

        struct PipelineDefaults
    {
        wgpu::TextureDescriptor depthTextureDesc;
        wgpu::Texture depthTexture;
        wgpu::TextureViewDescriptor depthTextureViewDesc;
        wgpu::TextureView depthTextureView;
        wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth24Plus;
        wgpu::DepthStencilState defaultDepthStencilState;
        wgpu::ColorTargetState defaultColorTarget;
        wgpu::BlendState defaultBlendState;
    };

    GLFWwindow* window;
    wgpu::Device device;
    wgpu::Queue queue;
    wgpu::Surface surface;
    wgpu::TextureFormat surfaceFormat = wgpu::TextureFormat::Undefined;
    wgpu::TextureView targetView;
    wgpu::RenderPassEncoder renderPass;
    wgpu::CommandEncoder encoder;
    PipelineDefaults pipelineDefaults;

    Renderer::Renderer();
    void initializePipelineDefaults();

    Model createModel(const std::vector<VertexAttributes>& vertexData, const Material& material, const TransformUniforms& transformUniforms);
    Material createMaterial(const std::filesystem::path& shaderPath, const VisualUniforms& visualUniforms, uint64_t transformUniformSize, const wgpu::TextureView& texture, const wgpu::Sampler& sampler);
    wgpu::TextureView createTexture(const PixelData& pixelData);
    wgpu::Sampler createSampler();

    void beginFrame();
    void endFrame();
    void draw(const Model& model);

    void createRenderPass();
    wgpu::TextureView getNextSurfaceTextureView();
    wgpu::RenderPipeline createPipeline(const wgpu::ShaderModule& shaderModule, const wgpu::PipelineLayout& pipelineLayout);
    wgpu::Buffer writeUniformBuffer(const Model& model);
    wgpu::BindGroup createBindGroups(wgpu::Buffer uniformBuffer, const Material& material);

    void terminate();
    bool isRunning();
    wgpu::Adapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options);
    wgpu::Device requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);
};
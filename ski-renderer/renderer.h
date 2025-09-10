#include "utils/wgpu-utils.h"
#include "utils/ResourceManager.h"
#include "utils/glfw3webgpu.h"

#define WEBGPU_CPP_IMPLEMENTATION
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

#include <webgpu/webgpu.hpp>

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <corecrt_math_defines.h>

struct Vertex
{
    float x;
    float y;
    float z;
};

struct Uniforms
{
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];
};

struct Material
{
    std::string shaderPath;
    Uniforms uniforms;
};

struct CircleMaterial : Material
{
    std::string shaderPath;
    Uniforms uniforms;
};

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

public:
    // Initialize everything and return true if it went all right
    bool
    Initialize();

    // Uninitialize everything that was initialized
    void Terminate();

    // Draw a frame and handle events
    void MainLoop();

    // Return true as long as the main loop should keep on running
    bool IsRunning();

    void draw(Material material);

private:
    wgpu::TextureView GetNextSurfaceTextureView();

    // Substep of Initialize() that creates the render pipeline
    void InitializePipelineDefaults();
    void InitializeBuffers();
    void InitializeBindGroups();
    void updateUniforms();

    void updateBuffers();
    wgpu::ShaderModule createShaderModule(Material material);
    void createPipeline(wgpu::ShaderModule shaderModule);

private:
    // We put here all the variables that are shared between init and main loop
    GLFWwindow *window;
    wgpu::Device device;
    wgpu::Queue queue;
    wgpu::Surface surface;
    wgpu::TextureFormat surfaceFormat = wgpu::TextureFormat::Undefined;
    wgpu::RenderPipeline pipeline;
    wgpu::Buffer pointBuffer;
    wgpu::Buffer indexBuffer;
    uint32_t indexCount;
    wgpu::Buffer uniformBuffer;

    wgpu::BindGroup bindGroup;

    Uniforms uniforms;

    PipelineDefaults pipelineDefaults;

    // std::vector<Vertex> vertices;
    // std::vector<uint16_t> indices;
};

bool Renderer::Initialize()
{
    // Open window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(640, 480, "Learn WebGPU", nullptr, nullptr);

    wgpu::Instance instance = wgpuCreateInstance(nullptr);

    std::cout << "Requesting adapter..." << std::endl;
    surface = glfwGetWGPUSurface(instance, window);
    wgpu::RequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = surface;
    wgpu::Adapter adapter = requestAdapterSync(instance, &adapterOpts);
    std::cout << "Got adapter: " << adapter << std::endl;

    wgpuInstanceRelease(instance);

    std::cout << "Requesting device..." << std::endl;
    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = "My Device";
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "The default queue";

    device = requestDeviceSync(adapter, &deviceDesc);
    std::cout << "Got device: " << device << std::endl;

    auto onDeviceError = [](WGPUErrorType type, char const *message, void * /* pUserData */)
    {
        std::cout << "Uncaptured device error: type " << type;
        if (message)
            std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);

    queue = wgpuDeviceGetQueue(device);

    // Configure the surface
    wgpu::SurfaceConfiguration config = {};
    config.nextInChain = nullptr;

    // Configuration of the textures created for the underlying swap chain
    config.width = 640;
    config.height = 480;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    wgpu::SurfaceCapabilities capabilities;
    wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);
    surfaceFormat = capabilities.formats[0];
    config.format = surfaceFormat;

    // And we do not need any particular view format:
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.device = device;
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    wgpuSurfaceConfigure(surface, &config);

    // Release the adapter only after it has been fully utilized
    wgpuAdapterRelease(adapter);

    InitializePipelineDefaults();
    InitializeBuffers();
    InitializeBindGroups();
    return true;
}

void Renderer::Terminate()
{
    wgpuBindGroupRelease(bindGroup);
    wgpuPipelineLayoutRelease(pipelineDefaults.defaultLayout);
    wgpuBindGroupLayoutRelease(pipelineDefaults.defaultBindGroupLayout);
    wgpuBufferRelease(uniformBuffer);
    wgpuBufferRelease(pointBuffer);
    wgpuBufferRelease(indexBuffer);
    wgpuTextureViewRelease(pipelineDefaults.depthTextureView);
    wgpuTextureDestroy(pipelineDefaults.depthTexture);
    wgpuTextureRelease(pipelineDefaults.depthTexture);
    wgpuRenderPipelineRelease(pipeline);
    wgpuSurfaceUnconfigure(surface);
    wgpuQueueRelease(queue);
    wgpuSurfaceRelease(surface);
    wgpuDeviceRelease(device);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::MainLoop()
{
    glfwPollEvents();

    Material material = {"shaders/shader.wgsl", uniforms};
    auto s = createShaderModule(material);
    createPipeline(s);

    // Update uniform buffer
    float time = static_cast<float>(glfwGetTime());
    // Only update the 1-st float of the buffer
    wgpuQueueWriteBuffer(queue, uniformBuffer, offsetof(Uniforms, time), &time, sizeof(float));

    // Get the next target texture view
    wgpu::TextureView targetView = GetNextSurfaceTextureView();
    if (!targetView)
        return;

    // Create a command encoder for the draw call
    wgpu::CommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = "My command encoder";
    wgpu::CommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

    // Create the render pass that clears the screen with our color
    wgpu::RenderPassDescriptor renderPassDesc = {};
    renderPassDesc.nextInChain = nullptr;

    // The attachment part of the render pass descriptor describes the target texture of the pass
    wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
    renderPassColorAttachment.view = targetView;
    renderPassColorAttachment.resolveTarget = nullptr;
    renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
    renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
    renderPassColorAttachment.clearValue = wgpu::Color{0.05, 0.05, 0.05, 1.0};
#ifndef WEBGPU_BACKEND_WGPU
    renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif

    wgpu::RenderPassDepthStencilAttachment depthStencilAttachment = {};
    depthStencilAttachment.view = pipelineDefaults.depthTextureView;
    depthStencilAttachment.depthClearValue = 1.0f;
    depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
    depthStencilAttachment.depthReadOnly = false;
    depthStencilAttachment.stencilClearValue = 0;
    depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Undefined;
    depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Undefined;
    depthStencilAttachment.stencilReadOnly = true;

    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &renderPassColorAttachment;
    renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
    renderPassDesc.timestampWrites = nullptr;

    wgpu::RenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

    // Select which render pipeline to use
    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);

    // Set vertex buffer while encoding the render pass
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, pointBuffer, 0, wgpuBufferGetSize(pointBuffer));

    // The second argument must correspond to the choice of uint16_t or uint32_t
    // we've done when creating the index buffer.
    wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer, wgpu::IndexFormat::Uint16, 0, wgpuBufferGetSize(indexBuffer));

    // Set binding group here!
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);

    uniforms.time = static_cast<float>(glfwGetTime());
    // Only update the 1st float of the buffer
    wgpuQueueWriteBuffer(queue, uniformBuffer, offsetof(Uniforms, time), &uniforms.time, sizeof(Uniforms::time));
    float angle1 = uniforms.time;
    glm::mat4x4 scale = glm::scale(glm::mat4x4(1.0), glm::vec3(0.3f));
    glm::mat4x4 transpose = glm::translate(glm::mat4x4(1.0), glm::vec3(0.5, 0.0, 0.0));
    glm::mat4x4 rotation = glm::rotate(glm::mat4x4(1.0), angle1, glm::vec3(0.0, 0.0, 1.0));
    uniforms.modelMatrix = rotation * transpose * scale;
    wgpuQueueWriteBuffer(queue, uniformBuffer, offsetof(Uniforms, modelMatrix), &uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));
    // updateUniforms();

    // Replace `draw()` with `drawIndexed()` and `vertexCount` with `indexCount`
    // The extra argument is an offset within the index buffer.
    wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);

    // Encode and submit the render pass
    wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = "Command buffer";
    wgpu::CommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
    wgpuCommandEncoderRelease(encoder);

    wgpuQueueSubmit(queue, 1, (WGPUCommandBuffer *)&command);
    wgpuCommandBufferRelease(command);

    // At the end of the frame
    wgpuTextureViewRelease(targetView);
#ifndef __EMSCRIPTEN__
    wgpuSurfacePresent(surface);
#endif

#if defined(WEBGPU_BACKEND_DAWN)
    wgpuDeviceTick(device);
#elif defined(WEBGPU_BACKEND_WGPU)
    wgpuDevicePoll(device, false, nullptr);
#endif
}

bool Renderer::IsRunning()
{
    return !glfwWindowShouldClose(window);
}

wgpu::TextureView Renderer::GetNextSurfaceTextureView()
{
    // Get the surface texture
    wgpu::SurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success)
    {
        return nullptr;
    }

    // Create a view for this surface texture
    wgpu::TextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = "Surface texture view";
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
    viewDescriptor.dimension = wgpu::TextureViewDimension::_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = wgpu::TextureAspect::All;
    wgpu::TextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

#ifndef WEBGPU_BACKEND_WGPU
    // We no longer need the texture, only its view
    // (NB: with wgpu-native, surface textures must not be manually released)
    wgpuTextureRelease(surfaceTexture.texture);
#endif // WEBGPU_BACKEND_WGPU

    return targetView;
}

void Renderer::InitializePipelineDefaults()
{
    pipelineDefaults = {};

    pipelineDefaults.defaultBlendState = {};
    pipelineDefaults.defaultBlendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    pipelineDefaults.defaultBlendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    pipelineDefaults.defaultBlendState.color.operation = wgpu::BlendOperation::Add;
    pipelineDefaults.defaultBlendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
    pipelineDefaults.defaultBlendState.alpha.dstFactor = wgpu::BlendFactor::One;
    pipelineDefaults.defaultBlendState.alpha.operation = wgpu::BlendOperation::Add;

    pipelineDefaults.defaultColorTarget = {};
    pipelineDefaults.defaultColorTarget.format = surfaceFormat;
    pipelineDefaults.defaultColorTarget.blend = &pipelineDefaults.defaultBlendState;
    pipelineDefaults.defaultColorTarget.writeMask = WGPUColorWriteMask_All;

    pipelineDefaults.depthTextureFormat = wgpu::TextureFormat::Depth24Plus;

    pipelineDefaults.defaultDepthStencilState = wgpu::Default;
    pipelineDefaults.defaultDepthStencilState.depthCompare = wgpu::CompareFunction::Less;
    pipelineDefaults.defaultDepthStencilState.depthWriteEnabled = true;
    pipelineDefaults.defaultDepthStencilState.format = pipelineDefaults.depthTextureFormat;
    pipelineDefaults.defaultDepthStencilState.stencilReadMask = 0;
    pipelineDefaults.defaultDepthStencilState.stencilWriteMask = 0;

    pipelineDefaults.depthTextureDesc = {};
    pipelineDefaults.depthTextureDesc.nextInChain = nullptr;
    pipelineDefaults.depthTextureDesc.label = nullptr;
    pipelineDefaults.depthTextureDesc.dimension = wgpu::TextureDimension::_2D;
    pipelineDefaults.depthTextureDesc.format = pipelineDefaults.depthTextureFormat;
    pipelineDefaults.depthTextureDesc.mipLevelCount = 1;
    pipelineDefaults.depthTextureDesc.sampleCount = 1;
    pipelineDefaults.depthTextureDesc.size = {640, 480, 1};
    pipelineDefaults.depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
    pipelineDefaults.depthTextureDesc.viewFormatCount = 1;
    pipelineDefaults.depthTextureDesc.viewFormats = (WGPUTextureFormat *)&pipelineDefaults.depthTextureFormat;
    pipelineDefaults.depthTexture = wgpuDeviceCreateTexture(device, &pipelineDefaults.depthTextureDesc);

    pipelineDefaults.depthTextureViewDesc = {};
    pipelineDefaults.depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
    pipelineDefaults.depthTextureViewDesc.baseArrayLayer = 0;
    pipelineDefaults.depthTextureViewDesc.arrayLayerCount = 1;
    pipelineDefaults.depthTextureViewDesc.baseMipLevel = 0;
    pipelineDefaults.depthTextureViewDesc.mipLevelCount = 1;
    pipelineDefaults.depthTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
    pipelineDefaults.depthTextureViewDesc.format = pipelineDefaults.depthTextureFormat;
    pipelineDefaults.depthTextureView = wgpuTextureCreateView(pipelineDefaults.depthTexture, &pipelineDefaults.depthTextureViewDesc);

    pipelineDefaults.defaultBindingLayout = wgpu::Default;
    pipelineDefaults.defaultBindingLayout.binding = 0;
    pipelineDefaults.defaultBindingLayout.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    pipelineDefaults.defaultBindingLayout.buffer.type = wgpu::BufferBindingType::Uniform;
    pipelineDefaults.defaultBindingLayout.buffer.minBindingSize = sizeof(Uniforms);

    pipelineDefaults.defaultBindGroupLayoutDesc = {};
    pipelineDefaults.defaultBindGroupLayoutDesc.nextInChain = nullptr;
    pipelineDefaults.defaultBindGroupLayoutDesc.entryCount = 1;
    pipelineDefaults.defaultBindGroupLayoutDesc.entries = &pipelineDefaults.defaultBindingLayout;
    pipelineDefaults.defaultBindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &pipelineDefaults.defaultBindGroupLayoutDesc);

    pipelineDefaults.defaultPipelineLayoutDescriptor = {};
    pipelineDefaults.defaultPipelineLayoutDescriptor.nextInChain = nullptr;
    pipelineDefaults.defaultPipelineLayoutDescriptor.bindGroupLayoutCount = 1;
    pipelineDefaults.defaultPipelineLayoutDescriptor.bindGroupLayouts = (WGPUBindGroupLayout *)&pipelineDefaults.defaultBindGroupLayout;
    pipelineDefaults.defaultLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineDefaults.defaultPipelineLayoutDescriptor);
}

void Renderer::InitializeBuffers()
{
    // Define data vectors, but without filling them in
    std::vector<float> pointData;
    std::vector<uint16_t> indexData;

    // Here we use the new 'loadGeometry' function:
    bool success = ResourceManager::loadGeometry("models/pyramid.txt", pointData, indexData, 3);

    // Check for errors
    if (!success)
    {
        std::cerr << "Could not load geometry!" << std::endl;
        exit(1);
    }

    // We now store the index count rather than the vertex count
    indexCount = static_cast<uint32_t>(indexData.size());

    // Create point buffer
    WGPUBufferDescriptor bufferDesc{};
    bufferDesc.nextInChain = nullptr;
    bufferDesc.size = pointData.size() * sizeof(float);
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex; // Vertex usage here!
    bufferDesc.mappedAtCreation = false;
    pointBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

    // Upload geometry data to the buffer
    wgpuQueueWriteBuffer(queue, pointBuffer, 0, pointData.data(), bufferDesc.size);

    // Create index buffer
    // (we reuse the bufferDesc initialized for the pointBuffer)
    bufferDesc.size = indexData.size() * sizeof(uint16_t);
    bufferDesc.size = (bufferDesc.size + 3) & ~3; // round up to the next multiple of 4
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ;
    indexBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

    wgpuQueueWriteBuffer(queue, indexBuffer, 0, indexData.data(), bufferDesc.size);

    bufferDesc.size = sizeof(Uniforms);

    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;

    bufferDesc.mappedAtCreation = false;
    uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

    // Upload the initial value of the uniforms
    glm::vec3 focalPoint(0.0, 0.0, -2.0);
    float angle1 = 2.0f; // arbitrary time
    float angle2 = 3.0f * (float)M_PI / 4.0f;
    float ratio = 640.0f / 480.0f;
    float focalLength = 2.0;
    float near = 0.01f;
    float far = 100.0f;
    glm::mat4x4 S = glm::scale(glm::mat4x4(1.0), glm::vec3(0.3f));
    glm::mat4x4 T1 = glm::translate(glm::mat4x4(1.0), glm::vec3(0.5, 0.0, 0.0));
    glm::mat4x4 R1 = glm::rotate(glm::mat4x4(1.0), angle1, glm::vec3(0.0, 0.0, 1.0));
    uniforms.modelMatrix = R1 * T1 * S;

    glm::mat4x4 R2 = glm::rotate(glm::mat4x4(1.0), -angle2, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4x4 T2 = glm::translate(glm::mat4x4(1.0), -focalPoint);
    uniforms.viewMatrix = T2 * R2;

    // Option C: A different way of using GLM extensions
    glm::mat4x4 M(1.0);
    M = glm::rotate(M, angle1, glm::vec3(0.0, 0.0, 1.0));
    M = glm::translate(M, glm::vec3(0.5, 0.0, 0.0));
    M = glm::scale(M, glm::vec3(0.3f));
    uniforms.modelMatrix = M;

    glm::mat4x4 V(1.0);
    V = glm::translate(V, -focalPoint);
    V = glm::rotate(V, -angle2, glm::vec3(1.0, 0.0, 0.0));
    uniforms.viewMatrix = V;

    float fov = 2 * glm::atan(1 / focalLength);
    uniforms.projectionMatrix = glm::perspective(fov, ratio, near, far);

    uniforms.time = 1.0f;
    uniforms.color = {0.0f, 1.0f, 0.4f, 1.0f};
    wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &uniforms, sizeof(Uniforms));
}

void Renderer::InitializeBindGroups()
{
    // Create a binding
    wgpu::BindGroupEntry binding{};
    binding.nextInChain = nullptr;
    // The index of the binding (the entries in bindGroupDesc can be in any order)
    binding.binding = 0;
    // The buffer it is actually bound to
    binding.buffer = uniformBuffer;
    // We can specify an offset within the buffer, so that a single buffer can hold
    // multiple uniform blocks.
    binding.offset = 0;
    // And we specify again the size of the buffer.
    binding.size = sizeof(Uniforms);
    //             ^^^^^^^^^^^^^^^^^^ This was 4 * sizeof(float)

    // A bind group contains one or multiple bindings
    wgpu::BindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = pipelineDefaults.defaultBindGroupLayout;
    // There must be as many bindings as declared in the layout!
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &binding;
    bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);
}

void Renderer::draw(Material material)
{
    wgpu::ShaderModule shaderModule = createShaderModule(material);
    createPipeline(shaderModule);
}

wgpu::ShaderModule Renderer::createShaderModule(Material material)
{
    std::ifstream file(material.shaderPath);
    if (!file.is_open())
    {
        return nullptr;
    }
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string shaderSource(size, ' ');
    file.seekg(0);
    file.read(shaderSource.data(), size);

    WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = shaderSource.c_str();

    WGPUShaderModuleDescriptor shaderDesc{};
#ifdef WEBGPU_BACKEND_WGPU
    shaderDesc.hintCount = 0;
    shaderDesc.hints = nullptr;
#endif
    shaderDesc.nextInChain = &shaderCodeDesc.chain;
    return wgpuDeviceCreateShaderModule(device, &shaderDesc);
}

void Renderer::createPipeline(wgpu::ShaderModule shaderModule)
{
    wgpu::RenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.nextInChain = nullptr;

    wgpu::VertexBufferLayout vertexBufferLayout{};

    std::vector<wgpu::VertexAttribute> vertexAttribs(1);

    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[0].offset = 0;

    vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
    vertexBufferLayout.attributes = vertexAttribs.data();
    vertexBufferLayout.arrayStride = 3 * sizeof(float);
    vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;

    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    wgpu::FragmentState fragmentState{};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    fragmentState.targetCount = 1;
    fragmentState.targets = &pipelineDefaults.defaultColorTarget;

    pipelineDesc.fragment = &fragmentState;

    pipelineDesc.depthStencil = &pipelineDefaults.defaultDepthStencilState;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    pipelineDesc.layout = pipelineDefaults.defaultLayout;

    pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

    wgpuShaderModuleRelease(shaderModule);
}
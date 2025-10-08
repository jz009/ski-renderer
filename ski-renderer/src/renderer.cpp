#pragma once
#include "includes_fwd.h"

#include "renderer.h"

Renderer::Renderer()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(Constants::WIDTH, Constants::HEIGHT, "Learn WebGPU", nullptr, nullptr);

    wgpu::Instance instance = wgpuCreateInstance(nullptr);

    surface = glfwGetWGPUSurface(instance, window);

    wgpu::RequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = surface;
    wgpu::Adapter adapter = requestAdapterSync(instance, &adapterOpts);

    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = "My Device";
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "The default queue";

    device = requestDeviceSync(adapter, &deviceDesc);

    auto onDeviceError = [](WGPUErrorType type, char const *message, void * /* pUserData */)
    {
        std::cout << "Uncaptured device error: type " << type;
        if (message)
            std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);

    queue = wgpuDeviceGetQueue(device);

    wgpu::SurfaceCapabilities capabilities;
    wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);
    surfaceFormat = capabilities.formats[0];

    wgpu::SurfaceConfiguration config = {};
    config.nextInChain = nullptr;
    config.width = Constants::WIDTH;
    config.height = Constants::HEIGHT;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.format = surfaceFormat;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.device = device;
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    wgpuSurfaceConfigure(surface, &config);

    wgpuInstanceRelease(instance);
    wgpuAdapterRelease(adapter);
    initializePipelineDefaults();
}

void Renderer::terminate()
{
    wgpuPipelineLayoutRelease(pipelineDefaults.defaultLayout);
    wgpuBindGroupLayoutRelease(pipelineDefaults.defaultBindGroupLayout);
    // wgpuBufferRelease(pointBuffer);
    // wgpuBufferRelease(indexBuffer);
    wgpuTextureViewRelease(pipelineDefaults.depthTextureView);
    wgpuTextureDestroy(pipelineDefaults.depthTexture);
    wgpuTextureRelease(pipelineDefaults.depthTexture);
    wgpuSurfaceUnconfigure(surface);
    wgpuQueueRelease(queue);
    wgpuSurfaceRelease(surface);
    wgpuDeviceRelease(device);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::createRenderPass()
{

    wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
    renderPassColorAttachment.view = targetView;
    renderPassColorAttachment.resolveTarget = nullptr;
    renderPassColorAttachment.loadOp = wgpu::LoadOp::Load;
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

    wgpu::RenderPassDescriptor renderPassDesc = {};
    renderPassDesc.nextInChain = nullptr;
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &renderPassColorAttachment;
    renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
    renderPassDesc.timestampWrites = nullptr;

    renderPass = encoder.beginRenderPass(renderPassDesc);
}

bool Renderer::isRunning()
{
    return !glfwWindowShouldClose(window);
}

wgpu::TextureView Renderer::getNextSurfaceTextureView()
{
    wgpu::SurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success)
    {
        return nullptr;
    }

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
    targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

#ifndef WEBGPU_BACKEND_WGPU
    wgpuTextureRelease(surfaceTexture.texture);
#endif

    return targetView;
}

void Renderer::initializePipelineDefaults()
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
    pipelineDefaults.depthTextureDesc.size = {Constants::WIDTH, Constants::HEIGHT, 1};
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

Material Renderer::createMaterial(const std::filesystem::path &shaderPath, const Uniforms &uniforms)
{
    std::ifstream file(shaderPath);
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string shaderSource(size, ' ');
    file.seekg(0);
    file.read(shaderSource.data(), size);

    wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc{};
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = shaderSource.c_str();

    wgpu::ShaderModuleDescriptor shaderDesc{};
#ifdef WEBGPU_BACKEND_WGPU
    shaderDesc.hintCount = 0;
    shaderDesc.hints = nullptr;
#endif
    shaderDesc.nextInChain = &shaderCodeDesc.chain;
    wgpu::ShaderModule shader = device.createShaderModule(shaderDesc);
    return {shader, uniforms};
}

Model Renderer::createModel(const std::vector<VertexAttributes>& vertexData, const Material &material)
{
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = vertexData.size() * sizeof(VertexAttributes);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;
    wgpu::Buffer vertexBuffer = device.createBuffer(bufferDesc);
    queue.writeBuffer(vertexBuffer, 0, vertexData.data(), bufferDesc.size);

    uint32_t vertexCount = static_cast<int>(vertexData.size());

    return Model{vertexBuffer, vertexCount, material};
}

void Renderer::writeUniformBuffer(const Material &material)
{
    // // Create index buffer
    // // (we reuse the bufferDesc initialized for the pointBuffer)
    // bufferDesc.size = indexData.size() * sizeof(uint16_t);
    // bufferDesc.size = (bufferDesc.size + 3) & ~3; // round up to the next multiple of 4
    // bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    // ;
    // indexBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

    // wgpuQueueWriteBuffer(queue, indexBuffer, 0, indexData.data(), bufferDesc.size);

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = sizeof(Uniforms);
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    bufferDesc.mappedAtCreation = false;
    uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

    wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &material.uniforms, sizeof(Uniforms));
}

void Renderer::initializeBindGroups()
{
    wgpu::BindGroupEntry binding{};
    binding.nextInChain = nullptr;
    binding.binding = 0;
    binding.buffer = uniformBuffer;
    binding.offset = 0;
    binding.size = sizeof(Uniforms);

    wgpu::BindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = pipelineDefaults.defaultBindGroupLayout;
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &binding;
    bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);
}

void Renderer::draw(const Model &model)
{
    writeUniformBuffer(model.material);
    initializeBindGroups();
    createPipeline(model.material.shader);

    renderPass.setPipeline(pipeline);
    renderPass.setVertexBuffer(0, model.buffer, 0, model.buffer.getSize());
    // renderPass.setIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint16, 0, indexBuffer.getSize());
    renderPass.setBindGroup(0, bindGroup, 0, nullptr);
    // renderPass.drawIndexed(indexCount, 1, 0, 0, 0);
    renderPass.draw(model.vertexCount, 1, 0, 0);
    uniformBuffer.release();
    bindGroup.release();
    pipeline.release();

}

void Renderer::beginFrame()
{
    glfwPollEvents();
    targetView = getNextSurfaceTextureView();
    encoder = device.createCommandEncoder();
    createRenderPass();
}

void Renderer::endFrame()
{
    renderPass.end();
    renderPass.release();
    wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = "Command buffer";
    wgpu::CommandBuffer command = encoder.finish(cmdBufferDescriptor);
    encoder.release();
    queue.submit(1, &command);
    command.release();
    targetView.release();

#ifndef __EMSCRIPTEN__
    wgpuSurfacePresent(surface);
#endif

#if defined(WEBGPU_BACKEND_DAWN)
    wgpuDeviceTick(device);
#elif defined(WEBGPU_BACKEND_WGPU)
    wgpuDevicePoll(device, false, nullptr);
#endif
}

void Renderer::createPipeline(const wgpu::ShaderModule& shaderModule)
{
    wgpu::RenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.nextInChain = nullptr;

    wgpu::VertexBufferLayout vertexBufferLayout{};

    std::vector<wgpu::VertexAttribute> vertexAttribs(3);

    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[0].offset = 0;

    vertexAttribs[1].shaderLocation = 1;
    vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[1].offset = offsetof(VertexAttributes, normal);

    vertexAttribs[2].shaderLocation = 2;
    vertexAttribs[2].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[2].offset = offsetof(VertexAttributes, color);

    vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
    vertexBufferLayout.attributes = vertexAttribs.data();
    vertexBufferLayout.arrayStride = sizeof(VertexAttributes);
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

    pipeline = device.createRenderPipeline(pipelineDesc);
}

wgpu::Adapter Renderer::requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options)
{
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message, void *pUserData)
    {
        UserData &userData = *reinterpret_cast<UserData *>(pUserData);
        if (status == WGPURequestAdapterStatus_Success)
        {
            userData.adapter = adapter;
        }
        else
        {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(
        instance,
        options,
        onAdapterRequestEnded,
        (void *)&userData);

    assert(userData.requestEnded);

    return userData.adapter;
}

wgpu::Device Renderer::requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor)
{
    struct UserData
    {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *pUserData)
    {
        UserData &userData = *reinterpret_cast<UserData *>(pUserData);
        if (status == WGPURequestDeviceStatus_Success)
        {
            userData.device = device;
        }
        else
        {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        onDeviceRequestEnded,
        (void *)&userData);

    assert(userData.requestEnded);

    return userData.device;
}
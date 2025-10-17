#pragma once
#include "includes_fwd.h"

#include "renderer.h"
#include "rendering_utils.h"

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
    adapterOpts.powerPreference = wgpu::PowerPreference::HighPerformance;
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

    auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */)
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
    renderPassColorAttachment.clearValue = wgpu::Color{ 0.05, 0.05, 0.05, 1.0 };
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
    pipelineDefaults.depthTextureDesc.size = { Constants::WIDTH, Constants::HEIGHT, 1 };
    pipelineDefaults.depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
    pipelineDefaults.depthTextureDesc.viewFormatCount = 1;
    pipelineDefaults.depthTextureDesc.viewFormats = (WGPUTextureFormat*)&pipelineDefaults.depthTextureFormat;
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
}

Material Renderer::createMaterial(const std::filesystem::path& shaderPath, const VisualUniforms& visualUniforms, uint64_t transformUniformSize, const wgpu::TextureView& texture, const wgpu::Sampler& sampler)
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
    
    std::vector<wgpu::BindGroupLayoutEntry>bindingLayoutEntries(4);
    bindingLayoutEntries[0] = wgpu::BindGroupLayoutEntry(wgpu::Default);
    bindingLayoutEntries[0].binding = 0;
    bindingLayoutEntries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    bindingLayoutEntries[0].buffer.type = wgpu::BufferBindingType::Uniform;
    bindingLayoutEntries[0].buffer.minBindingSize = sizeof(visualUniforms);

    bindingLayoutEntries[1] = wgpu::BindGroupLayoutEntry(wgpu::Default);
    bindingLayoutEntries[1].binding = 1;
    bindingLayoutEntries[1].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    bindingLayoutEntries[1].buffer.type = wgpu::BufferBindingType::Uniform;
    bindingLayoutEntries[1].buffer.minBindingSize = transformUniformSize;

    bindingLayoutEntries[2] = wgpu::BindGroupLayoutEntry(wgpu::Default);
    bindingLayoutEntries[2].binding = 2;
    bindingLayoutEntries[2].visibility = wgpu::ShaderStage::Fragment;
    bindingLayoutEntries[2].texture.sampleType = wgpu::TextureSampleType::Float;
    bindingLayoutEntries[2].texture.viewDimension = wgpu::TextureViewDimension::_2D;
    bindingLayoutEntries[2].texture.multisampled = 0;

    bindingLayoutEntries[3] = wgpu::BindGroupLayoutEntry(wgpu::Default);
    bindingLayoutEntries[3].binding = 3;
    bindingLayoutEntries[3].visibility = wgpu::ShaderStage::Fragment;
    bindingLayoutEntries[3].sampler.type = wgpu::SamplerBindingType::Filtering;

    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.nextInChain = nullptr;
    bindGroupLayoutDesc.entryCount = (uint32_t)bindingLayoutEntries.size();
    bindGroupLayoutDesc.entries = bindingLayoutEntries.data();
    wgpu::BindGroupLayout bindGroupLayout = device.createBindGroupLayout(bindGroupLayoutDesc);

    wgpu::PipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.nextInChain = nullptr;
    pipelineLayoutDesc.bindGroupLayoutCount = 1;
    pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
    wgpu::PipelineLayout pipelineLayout = device.createPipelineLayout(pipelineLayoutDesc);

    wgpu::RenderPipeline pipeline = createPipeline(shader, pipelineLayout);

    return { {pipeline, bindGroupLayout}, visualUniforms, texture, sampler };
}

Model Renderer::createModel(const std::vector<VertexAttributes>& vertexData, const Material& material, const TransformUniforms& transformUniforms)
{
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = vertexData.size() * sizeof(VertexAttributes);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;
    wgpu::Buffer vertexBuffer = device.createBuffer(bufferDesc);
    queue.writeBuffer(vertexBuffer, 0, vertexData.data(), bufferDesc.size);

    uint32_t vertexCount = (int)(vertexData.size());

    return Model(vertexBuffer, vertexCount, transformUniforms, material);
}

wgpu::TextureView Renderer::createTexture(const PixelData& pixelData) {
    wgpu::TextureDescriptor textureDesc = {};
    textureDesc.dimension = wgpu::TextureDimension::_2D;
    textureDesc.size = { pixelData.width, pixelData.height, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;

    wgpu::Texture texture = device.createTexture(textureDesc);

    wgpu::ImageCopyTexture destination = {};
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = wgpu::TextureAspect::All;

    wgpu::TextureDataLayout source = {};
    source.offset = 0;
    source.bytesPerRow = 4 * textureDesc.size.width;
    source.rowsPerImage = textureDesc.size.height;

    queue.writeTexture(destination, pixelData.pixels.data(), pixelData.pixels.size(), source, textureDesc.size);


    wgpu::TextureViewDescriptor textureViewDesc;
    textureViewDesc.aspect = wgpu::TextureAspect::All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
    textureViewDesc.format = textureDesc.format;
    return texture.createView(textureViewDesc);
}

wgpu::Sampler Renderer::createSampler() {
    wgpu::SamplerDescriptor samplerDesc;
    samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
    samplerDesc.magFilter = wgpu::FilterMode::Linear;
    samplerDesc.minFilter = wgpu::FilterMode::Linear;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 1.0f;
    samplerDesc.compare = wgpu::CompareFunction::Undefined;
    samplerDesc.maxAnisotropy = 1;
    return device.createSampler(samplerDesc);
}

wgpu::Buffer Renderer::writeUniformBuffer(const Model& model)
{
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = 512;
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    bufferDesc.label = "Packed Uniform Buffer";
    bufferDesc.mappedAtCreation = false;

    wgpu::Buffer buffer = device.createBuffer(bufferDesc);

    queue.writeBuffer(buffer, 0, &model.material.uniforms, sizeof(model.material.uniforms));
    queue.writeBuffer(buffer, 256, &model.transforms, sizeof(model.transforms));

    return buffer;
}

wgpu::BindGroup Renderer::createBindGroups(wgpu::Buffer uniformBuffer, const Material& material)
{
    std::vector<wgpu::BindGroupEntry> bindings(4);
    bindings[0].nextInChain = nullptr;
    bindings[0].binding = 0;
    bindings[0].buffer = uniformBuffer;
    bindings[0].offset = 0;
    bindings[0].size = 256;

    bindings[1].nextInChain = nullptr;
    bindings[1].binding = 1;
    bindings[1].buffer = uniformBuffer;
    bindings[1].offset = 256;
    bindings[1].size = 256;

    bindings[2].nextInChain = nullptr;
    bindings[2].binding = 2;
    bindings[2].textureView = material.texture;

    bindings[3].nextInChain = nullptr;
    bindings[3].binding = 3;
    bindings[3].sampler = material.sampler;

    wgpu::BindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = material.pipeline.bindGroupLayout;
    bindGroupDesc.entryCount = (uint32_t)bindings.size();
    bindGroupDesc.entries = bindings.data();
    return device.createBindGroup(bindGroupDesc);
}

void Renderer::beginFrame()
{
    glfwPollEvents();
    targetView = getNextSurfaceTextureView();
    encoder = device.createCommandEncoder();
    createRenderPass();
}

void Renderer::draw(const Model& model)
{
    wgpu::Buffer uniformBuffer = writeUniformBuffer(model);
    wgpu::BindGroup bindGroup = createBindGroups(uniformBuffer, model.material);
    renderPass.setPipeline(model.material.pipeline.pipeline);
    renderPass.setVertexBuffer(0, model.buffer, 0, model.buffer.getSize());
    renderPass.setBindGroup(0, bindGroup, 0, nullptr);
    renderPass.draw(model.vertexCount, 1, 0, 0);
    uniformBuffer.release();
    bindGroup.release();
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

wgpu::RenderPipeline Renderer::createPipeline(const wgpu::ShaderModule& shaderModule, const wgpu::PipelineLayout& pipelineLayout)
{
    wgpu::RenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.nextInChain = nullptr;

    wgpu::VertexBufferLayout vertexBufferLayout{};

    std::vector<wgpu::VertexAttribute> vertexAttribs(4);

    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[0].offset = 0;

    vertexAttribs[1].shaderLocation = 1;
    vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[1].offset = offsetof(VertexAttributes, normal);

    vertexAttribs[2].shaderLocation = 2;
    vertexAttribs[2].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[2].offset = offsetof(VertexAttributes, color);

    vertexAttribs[3].shaderLocation = 3;
    vertexAttribs[3].format = wgpu::VertexFormat::Float32x2;
    vertexAttribs[3].offset = offsetof(VertexAttributes, uv);

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

    pipelineDesc.layout = pipelineLayout;

    return device.createRenderPipeline(pipelineDesc);
}

wgpu::Adapter Renderer::requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options)
{
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* pUserData)
        {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
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
        (void*)&userData);

    assert(userData.requestEnded);

    return userData.adapter;
}

wgpu::Device Renderer::requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor)
{
    struct UserData
    {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* pUserData)
        {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
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
        (void*)&userData);

    assert(userData.requestEnded);

    return userData.device;
}
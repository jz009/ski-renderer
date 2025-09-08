
#pragma once
#include <vector>
#include <filesystem>
#include <webgpu/webgpu.h>

class ResourceManager
{
public:
    /**
     * Load a file from `path` using our ad-hoc format and populate the `pointData`
     * and `indexData` vectors.
     */
    static bool loadGeometry(
        const std::filesystem::path &path,
        std::vector<float> &pointData,
        std::vector<uint16_t> &indexData);

    /**
     * Create a shader module for a given WebGPU `device` from a WGSL shader source
     * loaded from file `path`.
     */
    static WGPUShaderModule loadShaderModule(
        const std::filesystem::path &path,
        WGPUDevice device);
};

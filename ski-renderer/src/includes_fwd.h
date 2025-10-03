#pragma once

#include "glfw3webgpu/glfw3webgpu.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cmath>
#include <fstream>
#include <sstream>
#include <optional>
#include <tiny_obj_loader.h>
#include <webgpu/webgpu.hpp>

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <array>
#include <corecrt_math_defines.h>
#include <unordered_map>
#include <limits>
#include <queue>
#include <bitset>

struct Entity;
struct Scene;
struct AABB;
struct BoxCollider;
struct Scene;
struct Camera;
struct Raycast;

struct Constants
{
    static constexpr float D2R = (float)M_PI / 180.f;
    static constexpr uint32_t WIDTH = 1500;
    static constexpr uint32_t HEIGHT = 1000;

    static const std::string Constants::mCUBE;
	static const std::string Constants::sDEFAULT;
};

#ifndef HELPERS
#define HELPERS
inline void printVec3(glm::vec3 vec)
{
    printf("%f, %f, %f\n", vec.x, vec.y, vec.z);
}

inline void printVec2(glm::vec2 vec)
{
    printf("%f, %f\n", vec.x, vec.y);
}
#endif
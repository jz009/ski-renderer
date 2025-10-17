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
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <set>

#define DISALLOW_IMPLICIT_COPIES(T) \
    T(T&&) = default; \
    T(const T&) = delete; \
    T operator=(const T&) const = delete; 

struct Scene;
struct Input;
struct Editor;
struct Entity;
struct Camera;
struct Model;
struct AABB;
struct AABB2D;
struct Transform;
struct Material;
struct VertexAttributes;
struct Uniforms;
struct Raycast;
struct NavMesh;
struct BoxCollider;
struct RayCollision;
struct Movement;
enum struct EntityType;

struct Constants
{
    static constexpr float D2R = (float)M_PI / 180.f;
    static constexpr float MAX_RADIANS = (float)M_PI * 2.0f;
    static constexpr uint32_t WIDTH = 1500;
    static constexpr uint32_t HEIGHT = 1000;
    static constexpr glm::vec4 EDIT_COLOR = { 1.0f, 0.0f, 0.0f, 1.0f };

    static const std::string Constants::mCUBE;
    static const std::string Constants::sDEFAULT;
};

struct Time
{
    static float deltaTime;
    static std::chrono::steady_clock::time_point lastFrame;
    static std::chrono::steady_clock::time_point thisFrame;
};

#ifndef HELPERS
#define HELPERS
inline void newline()
{
    printf("\n");
}

inline void printVec4(glm::vec4 vec)
{
    printf("%f, %f, %f, %f\n", vec.x, vec.y, vec.z, vec.a);
}

inline void printVec3(glm::vec3 vec)
{
    printf("%f, %f, %f\n", vec.x, vec.y, vec.z);
}

inline void printVec2(glm::vec2 vec)
{
    printf("%f, %f\n", vec.x, vec.y);
}

inline void printBool(bool b)
{
    if (b) {
        printf("true\n");
    }
    else {
        printf("false\n");
    }
}

inline bool areClose(float a, float b) {
    return abs(a - b) < .01f;
}

#endif
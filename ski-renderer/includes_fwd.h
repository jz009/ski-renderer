#pragma once
#include "utils/glfw3webgpu.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "external/glm/glm.hpp"
#include "external/glm/ext.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <optional>
#include "external/tiny_obj_loader.h"

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
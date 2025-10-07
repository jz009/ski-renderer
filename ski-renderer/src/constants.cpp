#pragma once
#include "includes_fwd.h"

const std::string Constants::mCUBE = "models/cube.obj";
const std::string Constants::sDEFAULT = "shaders/shader.wgsl";

std::chrono::steady_clock::time_point Time::lastFrame = std::chrono::high_resolution_clock::now();;
std::chrono::steady_clock::time_point Time::thisFrame = std::chrono::high_resolution_clock::now();;
float Time::deltaTime = 0;
#pragma once
#include "utils/wgpu-utils.h"
#include "utils/ResourceManager.h"
#include "utils/glfw3webgpu.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <math.h>

#include <webgpu/webgpu.h>
#ifdef WEBGPU_BACKEND_WGPU
#include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include "renderer.h"
#include "shared.h"

struct Engine
{
public:
	// Initialize everything and return true if it went all right
	static Engine Initialize(Renderer r);
};

int main()
{
	// Engine engine;
	Renderer renderer;
	// Engine engine = Engine::Initialize(renderer);

	if (!renderer.initialize())
	{
		return 1;
	}

	Uniforms u = renderer.getDefaultUniforms();
	Model MvertexData = renderer.createModel3D("models/mammoth.obj", "shaders/shader.wgsl", u);
	Model PvertexData = renderer.createModel3D("models/pyramid.obj", "shaders/shader.wgsl", u);
	Model plane = renderer.createModel2D("models/square.txt", "shaders/planeShader.wgsl", u);

#ifdef __EMSCRIPTEN__
	// Equivalent of the main loop when using Emscripten:
	auto callback = [](void *arg)
	{
		Renderer *prenderer = reinterpret_cast<Renderer *>(arg);
		prenderer->MainLoop(); // 4. We can use the Renderer object
	};
	emscripten_set_main_loop_arg(callback, &renderer, 0, true);
#else  // __EMSCRIPTEN__
	while (renderer.isRunning())
	{

		renderer.beginFrame();
		// renderer.MainLoop();
		//  renderer.draw({"shaders/circleShader.wgsl", "models/square.txt", {}});
		// MvertexData.material.uniforms = renderer.getDefaultUniforms();
		// PvertexData.material.uniforms = renderer.getDefaultUniforms();
		// renderer.draw(MvertexData);
		// renderer.draw(PvertexData);
		renderer.draw(plane);
		renderer.endFrame();
	}
#endif // __EMSCRIPTEN__

	renderer.terminate();

	return 0;
}

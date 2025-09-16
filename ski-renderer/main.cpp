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
#include "engine.h"


int main()
{
	// Engine engine;
	Renderer renderer;
	GLFWwindow* window = renderer.window;
	Engine engine(window);

	Uniforms uniform = renderer.getDefaultUniforms();
	engine.models.push_back(renderer.createModel3D(mCUBE, sDEFAULT, uniform));
	int count = 0;

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
		count = count + 1;
		renderer.beginFrame();
		for (Model model : engine.models) {
			renderer.draw(model);
		}
		renderer.endFrame();
	}
#endif // __EMSCRIPTEN__

	renderer.terminate();

	return 0;
}

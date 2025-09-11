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



struct Engine
{
public:
	// Initialize everything and return true if it went all right
	static Engine Initialize(Renderer r);


	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// void createCircle(float radius, float posX, float posY)
	// {
	// 	int segments = 16;
	// 	float angle_step = 2 * PI / segments;

	// 	auto i = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 9, 0, 9, 10, 0, 10, 11};
	// 	indices.insert(indices.end(), i.begin(), i.end());
	// 	vertices.push_back({posX, posY, 0.0f});
	// 	for (int i = 0; i < segments; ++i)
	// 	{
	// 		float angle = i * angle_step;
	// 		vertices.push_back({radius * cos(angle),
	// 							radius * sin(angle),
	// 							0.0f});
	// 	}
	// }

};

int main()
{
	Engine engine;
	Renderer renderer;
	// Engine engine = Engine::Initialize(renderer);

	if (!renderer.Initialize())
	{
		return 1;
	}

#ifdef __EMSCRIPTEN__
	// Equivalent of the main loop when using Emscripten:
	auto callback = [](void *arg)
	{
		Renderer *prenderer = reinterpret_cast<Renderer *>(arg);
		prenderer->MainLoop(); // 4. We can use the Renderer object
	};
	emscripten_set_main_loop_arg(callback, &renderer, 0, true);
#else  // __EMSCRIPTEN__
	while (renderer.IsRunning())
	{
		renderer.beginFrame();
		//renderer.MainLoop();
		renderer.draw({"shaders/circleShader.wgsl", "models/square.txt", {}});
		renderer.draw({"shaders/shader.wgsl", "models/square.txt", {}});
		renderer.endFrame();
	}
#endif // __EMSCRIPTEN__

	renderer.Terminate();

	return 0;
}

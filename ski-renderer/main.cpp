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
	GLFWwindow* w = renderer.getWindow();
	Engine engine(w);

	Uniforms u = renderer.getDefaultUniforms();
	Model MvertexData = renderer.createModel3D("models/mammoth.obj", "shaders/shader.wgsl", u);
	Model PvertexData = renderer.createModel3D("models/cube.obj", "shaders/shader.wgsl", u);
	Model plane = renderer.createModel2D("models/square.txt", "shaders/planeShader.wgsl", u);
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
		// renderer.MainLoop();
		//  renderer.draw({"shaders/circleShader.wgsl", "models/square.txt", {}});
		// MvertexData.material.uniforms = renderer.getDefaultUniforms();
		PvertexData.material.uniforms = renderer.getDefaultUniforms();
		PvertexData.material.uniforms.modelMatrix = glm::translate(glm::mat4x4(1.0), glm::vec3(-2.0f, 5.0f, -5.0f));
		PvertexData.material.uniforms.modelMatrix = glm::rotate(PvertexData.material.uniforms.modelMatrix, glm::radians((float)(count % 360)), glm::vec3(1.0f, 1.0f, 0.0f));
		PvertexData.material.uniforms.modelMatrix = glm::scale(PvertexData.material.uniforms.modelMatrix, glm::vec3(6.0, 1.0, 7.0));
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		const Camera& camera = engine.getCamera();
		PvertexData.material.uniforms.viewMatrix = glm::lookAt(camera.cameraPos, camera.targetPos, upVector);

		//renderer.draw(MvertexData);
		renderer.draw(PvertexData);
		//renderer.draw(plane);
		renderer.endFrame();
	}
#endif // __EMSCRIPTEN__

	renderer.terminate();

	return 0;
}

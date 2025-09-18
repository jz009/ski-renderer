#pragma once
#include "utils/glfw3webgpu.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <math.h>

#include <webgpu/webgpu.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include "renderer.h"
#include "shared.h"
#include "input.h"

int main()
{
	Renderer renderer;
	GLFWwindow *window = renderer.window;
	getInput()->init(window);
	int count = 0;

	std::vector<std::unique_ptr<Entity>> entities;
	std::unique_ptr<Player> player = std::make_unique<Player>();
	player->model = renderer.createModel3D(mCUBE, sDEFAULT, getDefaultUniforms());
	player->moveable.targetPosition = glm::vec3(0.0, 0.0, 0.0);
	entities.push_back(std::move(player));

	while (renderer.isRunning())
	{
		count = count + 1;
		renderer.beginFrame();
		for (auto& entity : entities)
		{
			entity->onFrame();
		}
		while (!models.empty())
		{
			renderer.draw(models.front());
			models.pop();
		}
		renderer.endFrame();
	}

	renderer.terminate();

	return 0;
}

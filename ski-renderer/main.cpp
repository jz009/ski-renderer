#pragma once
#define WEBGPU_CPP_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "shared.h"
#include "renderer.h"
#include "input.h"
#include "entity.h"
#include "scene.h"

int main()
{
	Renderer renderer;
	GLFWwindow *window = renderer.window;
	getInput()->init(window);
	int count = 0;

	Scene scene = {};
	
	auto player = std::make_unique<Player>();
	auto terrain = std::make_unique<Terrain>();
	Material basic = renderer.createMaterial(Constants::sDEFAULT, Uniforms());
	ObjResult objResult = loadObj(Constants::mCUBE);
	Model cube = renderer.createModel(objResult.vertexData, basic);

	player->model = cube;
	player->transform = Transform(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.5, 0.0));

	terrain->model = cube;
	terrain->transform = Transform(glm::vec3(100.0, 1.0, 100.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	terrain->model.material.uniforms.color = {1.0, 0.0, 0.0, 1.0};

	scene.entities.push_back(std::move(terrain));
	scene.entities.push_back(std::move(player));
	

	while (renderer.isRunning())
	{
		count = count + 1;
		renderer.beginFrame();
		for (auto& entity : scene.entities)
		{
			entity->onFrame();
			auto model = entity->getModel();
			if (model) {
				renderer.draw(*model);
			}
		}
		renderer.endFrame();
		getInput()->clear();
	}

	renderer.terminate();

	return 0;
}

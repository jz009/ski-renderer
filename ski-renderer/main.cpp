#pragma once
#define WEBGPU_CPP_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
// #include "includes_fwd.h"

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
	terrain->model = cube;
	terrain->transform = Transform(glm::vec3(100.0, 1.0, 100.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));

	player->model = cube;
	player->transform = Transform(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.5, 0.0));

	// // terrain->model = renderer.createModel(mCUBE, sDEFAULT, getDefaultUniforms());
	// // terrain->model.scale = {100.0, 1.0, 100.0};
	// // terrain->model.material.uniforms.color = {1.0, 0.0, 0.0, 1.0};
	// // terrain->model.position = {0.0, 0.0, 0.0};
	// // terrain->model.offset = {0.0, -1.0, 0.0};
	// // player->model = renderer.createModel3D(mCUBE, sDEFAULT, getDefaultUniforms());
	// // player->moveable.targetPosition = glm::vec3(0.0, 0.0, 0.0);
	// // player->model.offset = glm::vec3(0.0, 0.5, 0.0);
	// // entities.push_back(std::move(terrain));
	// // entities.push_back(std::move(player));

	// while (renderer.isRunning())
	// {
	// 	count = count + 1;
	// 	renderer.beginFrame();
	// 	for (auto& entity : entities)
	// 	{
	// 		entity->onFrame();
	// 	}
	// 	while (!models.empty())
	// 	{
	// 		renderer.draw(models.front());
	// 		models.pop_front();
	// 	}
	// 	renderer.endFrame();
	// 	getInput()->clear();
	// }

	// renderer.terminate();

	return 0;
}

#pragma once
#define WEBGPU_CPP_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "renderer.h"
#include "input.h"
#include "entity.h"
#include "collision.h"
#include "camera.h"
#include "scene.h"

int main()
{
	Renderer renderer;
	GLFWwindow *window = renderer.window;
	getInput()->init(window);
	int count = 0;

	auto camera = std::make_shared<CircleBoundCamera>(20.0f);
	Scene scene;
	scene.camera = camera;
	
	auto player = std::make_shared<Player>();
	auto terrain = std::make_shared<Terrain>();
	auto terrain2 = std::make_shared<Terrain>();
	Material basic = renderer.createMaterial(Constants::sDEFAULT, Uniforms(*scene.camera));
	ObjResult cubeObj = loadObj(Constants::mCUBE);
	Model cube = renderer.createModel(cubeObj.vertexData, basic);
	BoxCollider collider = BoxCollider(cubeObj.box, std::vector{Layer::WALKABLE}, terrain);
	BoxCollider collider2 = BoxCollider(cubeObj.box, std::vector{Layer::WALKABLE}, terrain2);

	player->model = cube;
	player->transform = Transform(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	player->model.material.uniforms.color = {0.7f, 0.3f, 0.3f, 1.0};

	terrain->model = cube;
	terrain->transform = Transform(glm::vec3(10.0, 1.0, 10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	terrain->model.material.uniforms.color = {0.3f, 1.0f, 0.0, 1.0};

	terrain2->model = cube;
	terrain2->transform = Transform(glm::vec3(10.0, 1.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(10.0, -14.0, 3.0), glm::vec3(0.0, -1.0, 0.0));
	terrain2->model.material.uniforms.color = {0.3f, 0.6f, 0.5f, 1.0};

	scene.entities.push_back(terrain);
	scene.entities.push_back(player);
	scene.entities.push_back(terrain2);
	scene.colliders.push_back(collider);
	scene.colliders.push_back(collider2);
	

	while (renderer.isRunning())
	{
		count = count + 1;
		renderer.beginFrame();
		for (const auto& entity : scene.entities)
		{
			entity->onFrame(scene);
			auto model = entity->getModel();
			if (model) {
				renderer.draw(*model);
			}
		}
		for (auto& c : scene.colliders) {
			c.onFrame();
		}
		scene.camera->onFrame();
		renderer.endFrame();
		getInput()->clear();
	}

	renderer.terminate();

	return 0;
}
